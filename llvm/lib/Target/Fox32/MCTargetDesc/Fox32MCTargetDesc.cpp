#include "Fox32MCTargetDesc.h"
#include "Fox32TargetInfo.h"

#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

#define GET_REGINFO_MC_DESC

#include "Fox32GenRegisterInfo.inc"

static MCRegisterInfo* createFox32MCRegisterInfo(const Triple &TT) {
    MCRegisterInfo *X = new MCRegisterInfo();
    InitFox32MCRegisterInfo(X, Fox32::r0);
    return X;
}

extern "C" void LLVMInitializeFox32TargetMC() {
    Target *T = &getTheFox32Target();
    TargetRegistry::RegisterMCRegInfo(*T, createFox32MCRegisterInfo);
}
