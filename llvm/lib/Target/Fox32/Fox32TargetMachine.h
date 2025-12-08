#ifndef LLVM_LIB_TARGET_FOX32_FOX32TARGETMACHINE_H
#define LLVM_LIB_TARGET_FOX32_FOX32TARGETMACHINE_H

#include "Fox32Subtarget.h"
#include "Fox32TargetObjectFile.h"

#include "llvm/CodeGen/CodeGenTargetMachineImpl.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Target/TargetLoweringObjectFile.h"
#include "llvm/Target/TargetMachine.h"

using namespace llvm;

static const char *Fox32DataLayoutString =
    "e-m:m-p:32:32-i8:8:32-i16:16:32-i64:64-n32-S64";

static Reloc::Model getEffectiveRelocModel(std::optional<Reloc::Model> RM) {
  if (!RM) {
    return Reloc::Static;
  }
  return *RM;
}

namespace llvm {
class Fox32TargetMachine final : public CodeGenTargetMachineImpl {
  Fox32Subtarget Subtarget;
  std::unique_ptr<TargetLoweringObjectFile> TLOF;

public:
  Fox32TargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                     StringRef FS, const TargetOptions &Options,
                     std::optional<Reloc::Model> RM,
                     std::optional<CodeModel::Model> CM, CodeGenOptLevel OL,
                     bool JIT)
      : CodeGenTargetMachineImpl(T, Fox32DataLayoutString, TT, CPU, FS, Options,
                                 getEffectiveRelocModel(RM),
                                 getEffectiveCodeModel(CM, CodeModel::Medium),
                                 OL),
        Subtarget(TT, CPU, FS, *this), TLOF(new Fox32TargetObjectFile()) {
    initAsmInfo();
  }

  //   ~Fox32TargetMachine() override = default;

  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }

  const Fox32Subtarget *getSubtargetImpl(const Function &F) const override {
    return &Subtarget;
  }

  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;
};
} // namespace llvm

#endif
