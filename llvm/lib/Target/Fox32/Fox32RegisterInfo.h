///// Fox32RegisterInfo.h
//// This contains the Fox32 implementation of the TargetRegisterInfo class.

#ifndef LLVM_LIB_TARGET_FOX32_FOX32REGISTERINFO_H
#define LLVM_LIB_TARGET_FOX32_FOX32REGISTERINFO_H

#include "llvm/CodeGen/TargetRegisterInfo.h"

#define GET_REGINFO_HEADER
#include "Fox32GenRegisterInfo.inc"

namespace llvm {

class Fox32RegisterInfo final : public Fox32GenRegisterInfo {
public:
  Fox32RegisterInfo();
  const MCPhysReg* getCalleeSavedRegs(const MachineFunction *MF) const override;

  BitVector getReservedRegs(const MachineFunction &MF) const override;

  bool eliminateFrameIndex(MachineBasicBlock::iterator II,
                            int SPAdj, unsigned FIOperandNum,
                            RegScavenger *RS = nullptr) const override;

  Register getFrameRegister(const MachineFunction &MF) const override;
};

} // end namespace llvm



#endif
