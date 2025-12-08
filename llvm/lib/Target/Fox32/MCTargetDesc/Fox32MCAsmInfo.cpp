#include "Fox32MCAsmInfo.h"

#include "llvm/TargetParser/Triple.h"

using namespace llvm;

void Fox32MCAsmInfo::anchor() {}

Fox32MCAsmInfo::Fox32MCAsmInfo(const Triple &TT) {
  IsLittleEndian = true;
  AlignmentIsInBytes = true;
}
