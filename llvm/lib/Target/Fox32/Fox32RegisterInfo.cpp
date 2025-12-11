#include "Fox32RegisterInfo.h"
#include "Fox32FrameLowering.h"
#include "MCTargetDesc/Fox32MCTargetDesc.h"

#include "llvm/ADT/BitVector.h"
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
  //   static const MCPhysReg CSRList[] = {
  //       Fox32::rsp, // Stack pointer
  //       Fox32::rfp, // Frame pointer
  //       Fox32::r0,  // R0
  //   };
  static const MCPhysReg CSRList[] = {0};
  return CSRList;
}

// TODO: Are these correct?
BitVector Fox32RegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  static const MCPhysReg ReservedRegs[] = {
      Fox32::rsp,
      Fox32::rfp,
      Fox32::r31, // `loop` and `rloop` are hard-coded to use this register as
                  // the counter.
  };
  BitVector Reserved(getNumRegs());
  for (MCPhysReg Reg : ReservedRegs) {
    Reserved.set(Reg);
  }
  return Reserved;
}

bool Fox32RegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                            int SPAdj, unsigned FIOperandNum,
                                            RegScavenger *RS) const {
  return true;
}

Register Fox32RegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  return Fox32::rfp;
}
