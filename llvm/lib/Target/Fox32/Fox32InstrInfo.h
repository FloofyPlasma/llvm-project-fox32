#ifndef LLVM_LIB_TARGET_FOX32_FOX32INSTRINFO_H
#define LLVM_LIB_TARGET_FOX32_FOX32INSTRINFO_H

#include "Fox32.h"
#include "Fox32RegisterInfo.h"

#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "Fox32GenInstrInfo.inc"

namespace llvm {
class Fox32Subtarget;

class Fox32InstrInfo : public Fox32GenInstrInfo {
public:
  explicit Fox32InstrInfo(const Fox32Subtarget &STI);

  void storeRegToStackSlot(MachineBasicBlock &MBB,
                           MachineBasicBlock::iterator MI, Register SrcReg,
                           bool isKill, int FrameIndex,
                           const TargetRegisterClass *RC, Register VReg,
                           MachineInstr::MIFlag Flags) const override;

  void loadRegFromStackSlot(MachineBasicBlock &MBB,
                            MachineBasicBlock::iterator MI, Register DestReg,
                            int FrameIndex, const TargetRegisterClass *RC,
                            Register VReg,
                            MachineInstr::MIFlag Flags) const override;

protected:
  const Fox32Subtarget &Subtarget;
};
} // end namespace llvm

#endif
