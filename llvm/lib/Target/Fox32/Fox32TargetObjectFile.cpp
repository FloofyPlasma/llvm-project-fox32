
#include "Fox32TargetObjectFile.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCSectionELF.h"
#include "llvm/Target/TargetMachine.h"

using namespace llvm;

void Fox32TargetObjectFile::Initialize(MCContext &Ctx,
                                       const TargetMachine &TM) {
  TargetLoweringObjectFile::Initialize(Ctx, TM);

  DefaultSection = Ctx.getObjectFileInfo()->getTextSection();

  TextSection = DefaultSection;
  DataSection = DefaultSection;
  BSSSection = DefaultSection;
  ReadOnlySection = DefaultSection;
  StackSizesSection = nullptr;
}

MCSection *Fox32TargetObjectFile::getSectionForConstant(
    const DataLayout &DL, SectionKind Kind, const Constant *C,
    Align &Alignment) const {
  return DefaultSection;
}

MCSection *Fox32TargetObjectFile::getExplicitSectionGlobal(
    const GlobalObject *GO, SectionKind Kind, const TargetMachine &TM) const {
  return DefaultSection;
}

MCSection *Fox32TargetObjectFile::SelectSectionForGlobal(
    const GlobalObject *GO, SectionKind Kind, const TargetMachine &TM) const {
  return DefaultSection;
}
