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
  const MachineFrameInfo &MFI = MF.getFrameInfo();
  const Fox32FrameLowering *TFL = getFrameLowering(MF);

  int FI = MI.getOperand(FIOperandNum).getIndex();
  int Offset = MFI.getObjectOffset(FI) + SPAdj;

  Register BaseReg = getFrameRegister(MF);

  if (!TFL->hasFP(MF)) {
    Offset += MFI.getStackSize();
  }

  // Replace the frame index with the base register
  MI.getOperand(FIOperandNum).ChangeToRegister(BaseReg, false);

  // Check if there's an offset operand (there should be for MEMri addressing
  // mode)
  if (FIOperandNum + 1 < MI.getNumOperands()) {
    MachineOperand &OffOp = MI.getOperand(FIOperandNum + 1);
    if (OffOp.isImm()) {
      // Add the frame offset to the existing immediate offset
      OffOp.setImm(OffOp.getImm() + Offset);
    } else {
      // If the next operand isn't an immediate, we need to add a new offset
      // operand This shouldn't normally happen with properly formed
      // instructions
      MI.addOperand(MachineOperand::CreateImm(Offset));
    }
  } else {
    // No offset operand exists, add one
    MI.addOperand(MachineOperand::CreateImm(Offset));
  }

  return true;
}

Register Fox32RegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  return Fox32::rfp;
}
