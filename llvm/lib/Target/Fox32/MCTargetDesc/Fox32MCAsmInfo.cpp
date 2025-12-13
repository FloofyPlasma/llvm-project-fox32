#include "Fox32MCAsmInfo.h"

#include "llvm/TargetParser/Triple.h"

using namespace llvm;

void Fox32MCAsmInfo::anchor() {}

Fox32MCAsmInfo::Fox32MCAsmInfo(const Triple &TT) {
  IsLittleEndian = true;
  AlignmentIsInBytes = true;

  CommentString = ";";

  HasDotTypeDotSizeDirective = false;
  WeakRefDirective = nullptr;
  HasIdentDirective = false;

  HasSingleParameterDotFile = false;

  UsesELFSectionDirectiveForBSS = false;

  Data8bitsDirective = "data.8 ";
  Data16bitsDirective = "data.16 ";
  Data32bitsDirective = "data.32 ";

  AsciiDirective = nullptr;
  AscizDirective = nullptr;

  PrivateGlobalPrefix = "";
  PrivateLabelPrefix = ".L";
  LabelSuffix = ":";

  GlobalDirective = nullptr;
}
