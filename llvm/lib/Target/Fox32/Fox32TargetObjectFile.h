#ifndef LLVM_LIB_TARGET_FOX32_FOX32TARGETOBJECTFILE_H
#define LLVM_LIB_TARGET_FOX32_FOX32TARGETOBJECTFILE_H

#include "llvm/Target/TargetLoweringObjectFile.h"

namespace llvm {

class Fox32TargetObjectFile : public TargetLoweringObjectFile {
private:
  MCSection *DefaultSection;

public:
  Fox32TargetObjectFile() = default;
  ~Fox32TargetObjectFile() override = default;

  void Initialize(MCContext &Ctx, const TargetMachine &TM) override;

  MCSection *getSectionForConstant(const DataLayout &DL, SectionKind Kind,
                                   const Constant *C,
                                   Align &Alignment) const override;

  MCSection *getExplicitSectionGlobal(const GlobalObject *GO, SectionKind Kind,
                                      const TargetMachine &TM) const override;

  MCSection *SelectSectionForGlobal(const GlobalObject *GO, SectionKind Kind,
                                    const TargetMachine &TM) const override;
};

} // end namespace llvm

#endif
