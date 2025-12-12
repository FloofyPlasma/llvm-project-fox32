#include "Fox32TargetMachine.h"
#include "Fox32.h"
#include "TargetInfo/Fox32TargetInfo.h"

#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Compiler.h"
#include "llvm/TargetParser/Triple.h"

using namespace llvm;

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeFox32TargetMC();
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeFox32AsmPrinter();

extern "C" void LLVMInitializeFox32Target() {
  RegisterTargetMachine<Fox32TargetMachine> X(getTheFox32Target());

  LLVMInitializeFox32TargetMC();

  LLVMInitializeFox32AsmPrinter();

  initializeFox32DAGToDAGISelLegacyPass(*PassRegistry::getPassRegistry());
}

namespace {
class Fox32PassConfig : public TargetPassConfig {
public:
  Fox32PassConfig(Fox32TargetMachine &TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {}

  Fox32TargetMachine &getFox32TargetMachine() const {
    return getTM<Fox32TargetMachine>();
  }

  bool addInstSelector() override {
    addPass(createFox32ISelDagLegacy(getFox32TargetMachine(), getOptLevel()));
    return false;
  }

  void addPreEmitPass() override {}
};
} // end namespace

TargetPassConfig *Fox32TargetMachine::createPassConfig(PassManagerBase &PM) {
  return new Fox32PassConfig(*this, PM);
}
