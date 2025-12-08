#ifndef LLVM_LIB_TARGET_FOX32_FOX32TARGETOBJECTFILE_H
#define LLVM_LIB_TARGET_FOX32_FOX32TARGETOBJECTFILE_H

#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"

namespace llvm {
class TargetMachine;
class MCContext;
class Fox32TargetObjectFile final : public TargetLoweringObjectFileELF {
public:
  void Initialize(MCContext &Ctx, const TargetMachine &TM) override;
};
} // namespace llvm

#endif
