#ifndef LLVM_LIB_TARGET_FOX32_MCTARGETDESC_FOX32MCASMINFO_H
#define LLVM_LIB_TARGET_FOX32_MCTARGETDESC_FOX32MCASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
class Triple;

class Fox32MCAsmInfo final : public MCAsmInfoELF {
  void anchor() override;

public:
  explicit Fox32MCAsmInfo(const Triple &TheTriple);
};
} // end namespace llvm

#endif
