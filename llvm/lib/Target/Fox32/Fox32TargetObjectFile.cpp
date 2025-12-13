
#include "Fox32TargetObjectFile.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCSectionELF.h"
#include "llvm/Target/TargetMachine.h"

using namespace llvm;

void Fox32TargetObjectFile::Initialize(MCContext &Ctx,
                                       const TargetMachine &TM) {
  TargetLoweringObjectFile::Initialize(Ctx, TM);

  DefaultSection = static_cast<MCSection *>(Ctx.getELFSection(
      ".fox32", ELF::SHT_PROGBITS, ELF::SHF_EXECINSTR | ELF::SHF_ALLOC));

  TextSection = DefaultSection;
  DataSection = DefaultSection;
  BSSSection = DefaultSection;
  ReadOnlySection = DefaultSection;
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
