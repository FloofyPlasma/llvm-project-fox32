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

void Fox32InstrInfo::storeRegToStackSlot(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator MI, Register SrcReg,
    bool isKill, int FrameIndex, const TargetRegisterClass *RC, Register VReg,
    MachineInstr::MIFlag Flags) const {
  // TODO: Implement for real
}

void Fox32InstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB,
                                          MachineBasicBlock::iterator MI,
                                          Register DestReg, int FrameIndex,
                                          const TargetRegisterClass *RC,
                                          Register VReg,
                                          MachineInstr::MIFlag Flags) const {
  // TODO: Implement for real
}
