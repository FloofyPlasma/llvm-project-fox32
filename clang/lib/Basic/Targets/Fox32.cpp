//===--- Fox32.cpp - Implement Fox32 target feature support --------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements Fox32 TargetInfo objects.
//
//===----------------------------------------------------------------------===//

#include "Fox32.h"
#include "clang/Basic/MacroBuilder.h"

using namespace clang;
using namespace clang::targets;

void Fox32TargetInfo::getTargetDefines(const LangOptions &Opts,
                                       MacroBuilder &Builder) const {
  Builder.defineMacro("__FOX32__");
  Builder.defineMacro("__fox32__");
  Builder.defineMacro("__FOX32");

  Builder.defineMacro("__FOX32_ARCH__", "1");

  Builder.defineMacro("__LITTLE_ENDIAN__");
  Builder.defineMacro("__BYTE_ORDER__", "__ORDER_LITTLE_ENDIAN__");

  Builder.defineMacro("__SIZEOF_POINTER__", "4");
  Builder.defineMacro("__SIZEOF_INT__", "4");
  Builder.defineMacro("__SIZEOF_LONG__", "4");
  Builder.defineMacro("__SIZEOF_LONG_LONG__", "4");
  Builder.defineMacro("__SIZEOF_SHORT__", "2");

  Builder.defineMacro("__FOX32_REGISTERS__", "32");
}

ArrayRef<const char *> Fox32TargetInfo::getGCCRegNames() const {
  static const char *const GCCRegNames[] = {
      // General purpose registers r0-r31
      "r0",
      "r1",
      "r2",
      "r3",
      "r4",
      "r5",
      "r6",
      "r7",
      "r8",
      "r9",
      "r10",
      "r11",
      "r12",
      "r13",
      "r14",
      "r15",
      "r16",
      "r17",
      "r18",
      "r19",
      "r20",
      "r21",
      "r22",
      "r23",
      "r24",
      "r25",
      "r26",
      "r27",
      "r28",
      "r29",
      "r30",
      "r31",

      // Special registers
      "rsp",
      "resp",
      "rfp",
  };
  return llvm::ArrayRef(GCCRegNames);
}

ArrayRef<TargetInfo::GCCRegAlias> Fox32TargetInfo::getGCCRegAliases() const {
  static const TargetInfo::GCCRegAlias GCCRegAliases[] = {
      {{"sp"}, "rsp"},
      {{"fp"}, "rfp"},
  };
  return llvm::ArrayRef(GCCRegAliases);
}
