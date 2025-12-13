#include "Fox32MCTargetDesc.h"
#include "TargetInfo/Fox32TargetInfo.h"

#include "MCTargetDesc/Fox32MCAsmInfo.h"
#include "MCTargetDesc/Fox32MCInstPrinter.h"

#include "llvm/MC/MCDwarf.h"
#include "llvm/MC/MCELFStreamer.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

#define GET_INSTRINFO_MC_DESC
#define ENABLE_INSTR_PREDICATE_VERIFIER
#include "Fox32GenInstrInfo.inc"

#define GET_REGINFO_MC_DESC
#include "Fox32GenRegisterInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "Fox32GenSubtargetInfo.inc"

static MCRegisterInfo *createFox32MCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitFox32MCRegisterInfo(X, Fox32::r0);
  return X;
}

static MCSubtargetInfo *createFox32SubtargetInfo(const Triple &TT,
                                                 StringRef CPU, StringRef FS) {
  if (CPU.empty()) {
    CPU = "generic";
  }
  return createFox32MCSubtargetInfoImpl(TT, CPU, CPU, FS);
}

static MCAsmInfo *createFox32MCAsmInfo(const MCRegisterInfo &MRI,
                                       const Triple &TT,
                                       const MCTargetOptions &Options) {
  MCAsmInfo *X = new Fox32MCAsmInfo(TT);
  unsigned SP = MRI.getDwarfRegNum(Fox32::rsp, true);
  MCCFIInstruction Inst = MCCFIInstruction::createDefCfaRegister(nullptr, SP);
  X->addInitialFrameState(Inst);
  return X;
}

static MCInstrInfo *createFox32MCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitFox32MCInstrInfo(X);
  return X;
}

static MCInstPrinter *createFox32MCInstPrinter(const Triple &T,
                                               unsigned SyntaxVariant,
                                               const MCAsmInfo &MAI,
                                               const MCInstrInfo &MII,
                                               const MCRegisterInfo &MRI) {
  return new Fox32InstPrinter(MAI, MII, MRI);
}

extern "C" void LLVMInitializeFox32TargetMC() {
  Target *T = &getTheFox32Target();
  TargetRegistry::RegisterMCRegInfo(*T, createFox32MCRegisterInfo);
  TargetRegistry::RegisterMCSubtargetInfo(*T, createFox32SubtargetInfo);
  TargetRegistry::RegisterMCAsmInfo(*T, createFox32MCAsmInfo);
  TargetRegistry::RegisterMCInstrInfo(*T, createFox32MCInstrInfo);
  TargetRegistry::RegisterMCInstPrinter(*T, createFox32MCInstPrinter);
}
