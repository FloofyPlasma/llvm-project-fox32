#include "Fox32FrameLowering.h"
#include "Fox32InstrInfo.h"
#include "Fox32Subtarget.h"
#include "MCTargetDesc/Fox32MCTargetDesc.h"

#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/IR/Function.h"

using namespace llvm;

bool Fox32FrameLowering::hasFPImpl(const MachineFunction &MF) const {
  const MachineFrameInfo &MFI = MF.getFrameInfo();
  const TargetRegisterInfo *TRI =
      MF.getSubtarget<Fox32Subtarget>().getRegisterInfo();

  // Use frame pointer if:
  // - Frame size is not known at compile time (variable sized objects)
  // - We have variable sized allocas
  // - Frame pointer is explicitly required
  // - We have calls and need to save LR (return address handling)
  return MFI.hasVarSizedObjects() || MFI.isFrameAddressTaken() ||
         TRI->hasStackRealignment(MF) ||
         MF.getFunction().hasFnAttribute(Attribute::NoInline);
}

void Fox32FrameLowering::emitPrologue(MachineFunction &MF,
                                      MachineBasicBlock &MBB) const {
  MachineFrameInfo &MFI = MF.getFrameInfo();
  const Fox32InstrInfo &TII = *static_cast<const Fox32InstrInfo *>(
      MF.getSubtarget<Fox32Subtarget>().getInstrInfo());
  MachineBasicBlock::iterator MBBI = MBB.begin();
  DebugLoc DL = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();

  uint64_t StackSize = MFI.getStackSize();

  if (StackSize == 0 && !MFI.adjustsStack()) {
    return;
  }

  StackSize = alignTo(StackSize, getStackAlign());

  bool HasFP = hasFP(MF);

  unsigned SPReg = Fox32::rsp;
  unsigned FPReg = Fox32::rfp;

  // Step 1: Save the frame pointer if we are using one
  if (HasFP) {
    // push.32 rfp
    BuildMI(MBB, MBBI, DL, TII.get(Fox32::PUSH_32r))
        .addReg(FPReg, RegState::Kill)
        .setMIFlag(MachineInstr::FrameSetup);

    BuildMI(MBB, MBBI, DL, TII.get(Fox32::MOV_32rr))
        .addReg(SPReg)
        .setMIFlag(MachineInstr::FrameSetup);
  }

  // Step 2: Allocate stack space
  // sub.32 rsp, StackSize
  if (StackSize > 0) {
    BuildMI(MBB, MBBI, DL, TII.get(Fox32::SUB_32ri), SPReg)
        .addReg(SPReg)
        .addImm(StackSize)
        .setMIFlag(MachineInstr::FrameSetup);
  }

  // Step 3: Set up frame pointer
  if (HasFP) {
    // mov.32 rfp, rsp
    BuildMI(MBB, MBBI, DL, TII.get(Fox32::MOV_32rr), FPReg)
        .addReg(SPReg)
        .setMIFlag(MachineInstr::FrameSetup);
  }

  // NOTE: Callee-saved registers are automatically handled by
  // PrologEpilogInserter based on the CSR_Fox32 definition
}

void Fox32FrameLowering::emitEpilogue(MachineFunction &MF,
                                      MachineBasicBlock &MBB) const {
  MachineFrameInfo &MFI = MF.getFrameInfo();
  const Fox32InstrInfo &TII = *static_cast<const Fox32InstrInfo *>(
      MF.getSubtarget<Fox32Subtarget>().getInstrInfo());

  // Find the return instruction
  MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
  assert(MBBI != MBB.end() && "Returning block has no terminator");

  DebugLoc DL = MBBI->getDebugLoc();

  // Get the number of bytes to deallocate
  uint64_t StackSize = MFI.getStackSize();
  StackSize = alignTo(StackSize, getStackAlign());

  bool HasFP = hasFP(MF);

  unsigned SPReg = Fox32::rsp;
  unsigned FPReg = Fox32::rfp;

  // Step 1: Restore stack pointer from frame pointer (if using FP)
  if (HasFP) {
    // mov.32 rsp, rfp
    BuildMI(MBB, MBBI, DL, TII.get(Fox32::MOV_32rr), SPReg)
        .addReg(FPReg)
        .setMIFlag(MachineInstr::FrameDestroy);
  } else if (StackSize > 0) {
    // Step 2: Deallocate stack space (if not using FP)
    // add.32 rsp, StackSize
    BuildMI(MBB, MBBI, DL, TII.get(Fox32::ADD_32ri), SPReg)
        .addReg(SPReg)
        .addImm(StackSize)
        .setMIFlag(MachineInstr::FrameDestroy);
  }

  // Step 3: Restore frame pointer
  if (HasFP) {
    // pop.32 rfp
    BuildMI(MBB, MBBI, DL, TII.get(Fox32::POP_32r), FPReg)
        .setMIFlag(MachineInstr::FrameDestroy);
  }

  // NOTE: The actual 'ret' instruction is already in the MBB
  // NOTE: Callee-saved reisters are restored automatically by
  // PrologEpilogInserter based on the CSR_Fox32 definition
}

MachineBasicBlock::iterator Fox32FrameLowering::eliminateCallFramePseudoInstr(
    MachineFunction &MF, MachineBasicBlock &MBB,
    MachineBasicBlock::iterator MI) const {
  const Fox32InstrInfo &TII = *static_cast<const Fox32InstrInfo *>(
      MF.getSubtarget<Fox32Subtarget>().getInstrInfo());
  DebugLoc DL = MI->getDebugLoc();

  unsigned SPReg = Fox32::rsp;

  // Get the amount to adjust
  int Amount = MI->getOperand(0).getImm();

  if (Amount != 0) {
    // ADJCALLSTACKDOWN: Allocate space for outgoing arguments
    // sub.32 rsp, amount
    if (MI->getOpcode() == Fox32::ADJCALLSTACKDOWN) {
      BuildMI(MBB, MI, DL, TII.get(Fox32::SUB_32ri), SPReg)
          .addReg(SPReg)
          .addImm(Amount);
    }
    // ADJCALLSTACKUP: Deallocate space after call
    // add.32 rsp, Amount
    else if (MI->getOpcode() == Fox32::ADJCALLSTACKUP) {
      BuildMI(MBB, MI, DL, TII.get(Fox32::ADD_32ri), SPReg)
          .addReg(SPReg)
          .addImm(Amount);
    }
  }

  // Erase the pseudo instruction
  return MBB.erase(MI);
}
