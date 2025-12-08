#include "Fox32.h"
#include "Fox32TargetInfo.h"

#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Compiler.h"
#include "llvm/TargetParser/Triple.h"

using namespace llvm;

Target& llvm::getTheFox32Target() {
    static Target TheFox32Target;
    return TheFox32Target;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeFox32TargetInfo(){ 
    llvm::RegisterTarget<llvm::Triple::fox32> X(::getTheFox32Target(),
    "fox32", "Fox32 (32-bit little endian)", "Fox32");
}

extern "C" void LLVMInitializeFox32Target() {
    // TODO: Add initialize target
}
