//===--- Fox32.h - Declare Fox32 target feature support --------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares Fox32 TargetInfo objects.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_LIB_BASIC_TARGETS_FOX32_H
#define LLVM_CLANG_LIB_BASIC_TARGETS_FOX32_H

#include "clang/Basic/Builtins.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "llvm/Support/Compiler.h"
#include "llvm/TargetParser/Triple.h"

namespace clang {
namespace targets {

class LLVM_LIBRARY_VISIBILITY Fox32TargetInfo : public TargetInfo {
public:
  Fox32TargetInfo(const llvm::Triple &Triple, const TargetOptions &)
      : TargetInfo(Triple) {
    PointerWidth = PointerAlign = 32;
    IntWidth = IntAlign = 32;
    LongWidth = LongAlign = 32;
    LongLongWidth = LongLongAlign = 32;
    SizeType = UnsignedInt;
    PtrDiffType = SignedInt;
    IntPtrType = SignedInt;

    HasFloat128 = false;
    HasFloat16 = false;
    HasBFloat16 = false;
    HasFullBFloat16 = false;
    HasIbm128 = false;
    HasLongDouble = false;
    HasFPReturn = false;
    HasStrictFP = false;

    ShortWidth = ShortAlign = 16;

    // Data layout for Fox32
    // e = little endian
    // p:32:32 = 32-bit pointers with 32-bit alignment
    // i1:8:8 = i1 is 8-bit aligned
    // i8:8:8 = 8-bit integers with 8-bit alignment
    // i16:16:16 = 16-bit integers with 16-bit alignment
    // i32:32:32 = 32-bit integers with 32-bit alignment
    // a:0:32 = aggregates have ABI alignment of 32 bits
    // n8:16:32 = native integer widths are 8, 16, and 32 bits
    // S32 = stack alignment is 32 bits
    resetDataLayout(
        "e-p:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-a:0:32-n8:16:32-S32");

    // No inline asm variants
    NoAsmVariants = true;
  }

  void getTargetDefines(const LangOptions &Opts,
                        MacroBuilder &Builder) const override;

  SmallVector<Builtin::InfosShard> getTargetBuiltins() const override {
    return {};
  }

  std::string_view getClobbers() const override { return ""; }

  BuiltinVaListKind getBuiltinVaListKind() const override {
    // Use simple pointer-based va_list
    return TargetInfo::VoidPtrBuiltinVaList;
  }

  ArrayRef<const char *> getGCCRegNames() const override;

  ArrayRef<TargetInfo::GCCRegAlias> getGCCRegAliases() const override;

  bool validateAsmConstraint(const char *&Name,
                             TargetInfo::ConstraintInfo &Info) const override {
    switch (*Name) {
    case 'r': // General purpose register
      Info.setAllowsRegister();
      return true;
    }
    return false;
  }

  bool hasBitIntType() const override { return true; }
};

} // namespace targets
} // namespace clang

#endif // LLVM_CLANG_LIB_BASIC_TARGETS_FOX32_H
