#include "Fox32RegisterInfo.h"
#include "Fox32FrameLowering.h"
#include "MCTargetDesc/Fox32MCTargetDesc.h"

#include "llvm/ADT/BitVector.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"

using namespace llvm;

#define DEBUG_TYPE "fox32-reg-info"

#define GET_REGINFO_TARGET_DESC
#include "Fox32GenRegisterInfo.inc"

using namespace llvm;

Fox32RegisterInfo::Fox32RegisterInfo() : Fox32GenRegisterInfo(Fox32::r0) {}

// TODO: Are these correct?
const MCPhysReg *
Fox32RegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  return CSR_Fox32_SaveList;
}

const uint32_t *
Fox32RegisterInfo::getCallPreservedMask(const MachineFunction &MF,
                                        CallingConv::ID CC) const {
  return CSR_Fox32_RegMask;
}

// TODO: Are these correct?
BitVector Fox32RegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  BitVector Reserved(getNumRegs());

  // Stack pointer
  Reserved.set(Fox32::rsp);

  // Frame pointer
  Reserved.set(Fox32::rfp);

  // r31 (used by loop/rloop instructions)
  Reserved.set(Fox32::r31);

  return Reserved;
}

bool Fox32RegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                            int SPAdj, unsigned FIOperandNum,
                                            RegScavenger *RS) const {
  MachineInstr &MI = *II;
  MachineFunction &MF = *MI.getParent()->getParent();
  const Fox32FrameLowering *TFL = getFrameLowering(MF);

  int FrameIndex = MI.getOperand(FIOperandNum).getIndex();

  int Offset = MF.getFrameInfo().getObjectOffset(FrameIndex);

  Offset += MI.getOperand(FIOperandNum + 1).getImm();

  Register BaseReg = getFrameRegister(MF);

  // If we have a frame pointer we should caluclate offset from it
  if (TFL->hasFP(MF)) {
    BaseReg = Fox32::rfp;

    Offset = -Offset;
  } else {
    // No frame pointer gotta use the stack pointer
    BaseReg = Fox32::rsp;

    Offset += MF.getFrameInfo().getStackSize();
  }

  MI.getOperand(FIOperandNum).ChangeToRegister(BaseReg, false);

  MI.getOperand(FIOperandNum + 1).ChangeToImmediate(Offset);

  return false;
}

Register Fox32RegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  return Fox32::rfp;
}
