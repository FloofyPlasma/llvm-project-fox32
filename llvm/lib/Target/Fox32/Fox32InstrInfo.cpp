#include "Fox32InstrInfo.h"
#include "Fox32TargetMachine.h"
#include "MCTargetDesc/Fox32MCTargetDesc.h"

#include "llvm/CodeGen/MachineInstrBuilder.h"

using namespace llvm;

#define DEBUG_TYPE "fox32-instr-info"

#define GET_INSTRINFO_CTOR_DTOR
#include "Fox32GenInstrInfo.inc"

Fox32InstrInfo::Fox32InstrInfo(const Fox32Subtarget &STI)
    : Fox32GenInstrInfo(STI, *STI.getRegisterInfo(), Fox32::ADJCALLSTACKDOWN,
                        Fox32::ADJCALLSTACKUP,
                        /* CatchRetOpcode */ ~0u, /* ReturnOpcode */ ~0u),
      Subtarget(STI) {}

void Fox32InstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                 MachineBasicBlock::iterator MI,
                                 const DebugLoc &DL, Register DstReg,
                                 Register SrcReg, bool KillSrc,
                                 bool RenamableDeste, bool RenamableSrc) const {
  unsigned Opc;

  if (Fox32::GPR32RegClass.contains(DstReg, SrcReg)) {
    Opc = Fox32::MOV_32rr;
  } else if (Fox32::GPR16RegClass.contains(DstReg, SrcReg)) {
    Opc = Fox32::MOV_16rr;
  } else if (Fox32::GPR8RegClass.contains(DstReg, SrcReg)) {
    Opc = Fox32::MOV_8rr;
  } else {
    // For special registers or mixed register classes, use MOV.32
    Opc = Fox32::MOV_32rr;
  }

  BuildMI(MBB, MI, DL, get(Opc), DstReg)
      .addReg(SrcReg, getKillRegState(KillSrc));
}

void Fox32InstrInfo::storeRegToStackSlot(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator MI, Register SrcReg,
    bool isKill, int FrameIndex, const TargetRegisterClass *RC, Register VReg,
    MachineInstr::MIFlag Flags) const {

  DebugLoc DL;
  if (MI != MBB.end())
    DL = MI->getDebugLoc();

  MachineFunction &MF = *MBB.getParent();
  MachineFrameInfo &MFI = MF.getFrameInfo();

  unsigned Opc;
  if (Fox32::GPR32RegClass.hasSubClassEq(RC)) {
    Opc = Fox32::STORE_32mr;
  } else if (Fox32::GPR16RegClass.hasSubClassEq(RC)) {
    Opc = Fox32::STORE_16mr;
  } else if (Fox32::GPR8RegClass.hasSubClassEq(RC)) {
    Opc = Fox32::STORE_8mr;
  } else {
    llvm_unreachable("Cannot store this register to stack slot!");
  }

  // TODO: Implement frame index elimination
}

void Fox32InstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB,
                                          MachineBasicBlock::iterator MI,
                                          Register DestReg, int FrameIndex,
                                          const TargetRegisterClass *RC,
                                          Register VReg,
                                          MachineInstr::MIFlag Flags) const {
  DebugLoc DL;
  if (MI != MBB.end())
    DL = MI->getDebugLoc();

  unsigned Opc;
  if (Fox32::GPR32RegClass.hasSubClassEq(RC)) {
    Opc = Fox32::LOAD_32rm;
  } else if (Fox32::GPR16RegClass.hasSubClassEq(RC)) {
    Opc = Fox32::LOAD_16rm;
  } else if (Fox32::GPR8RegClass.hasSubClassEq(RC)) {
    Opc = Fox32::LOAD_8rm;
  } else {
    llvm_unreachable("Cannot load this register from stack slot!");
  }

  // TODO: Implement frame index elimination
}
