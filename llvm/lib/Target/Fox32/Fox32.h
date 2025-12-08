/// This is for global functions in the Fox32 target.
#ifndef LLVM_LIB_TARGET_FOX32_FOX32_H
#define LLVM_LIB_TARGET_FOX32_FOX32_H
#include "MCTargetDesc/Fox32MCTargetDesc.h"

#include "llvm/Pass.h"
#include "llvm/Support/CodeGen.h"

namespace llvm {
class FunctionPass;
class Fox32TargetMachine;

FunctionPass *createFox32ISelDagLegacy(Fox32TargetMachine &TM,
                                       CodeGenOptLevel OptLevel);

void initializeFox32DAGToDAGISelLegacyPass(PassRegistry &);
} // end namespace llvm

#endif
