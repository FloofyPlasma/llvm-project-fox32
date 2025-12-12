//===--  ----//
// This file defines the frame lowering for the Fox32 target
//===---------------------------------------------------------

#ifndef LLVM_LIB_TARGET_FOX32_FOX32FRAMELOWERING_H
#define LLVM_LIB_TARGET_FOX32_FOX32FRAMELOWERING_H

#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"

namespace llvm {
class Fox32Subtarget;

class Fox32FrameLowering : public TargetFrameLowering {
public:
  explicit Fox32FrameLowering(const Fox32Subtarget &STI, Align Alignment)
      : TargetFrameLowering(StackGrowsDown, Alignment, 0, Alignment) {}

  void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

  MachineBasicBlock::iterator
  eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB,
                                MachineBasicBlock::iterator MI) const override;

private:
  bool hasFPImpl(const MachineFunction &MF) const override;
};
} // end namespace llvm

#endif
