/// This file provides Fox32-specific target descriptions.
#ifndef LLVM_LIB_TARGET_FOX32_MCTARGETDESC_H
#define LLVM_LIB_TARGET_FOX32_MCTARGETDESC_H

// Include symbolic names for registers. This includes the enum
// for register to register number mapping. (Fox32::r0 etc) and
// the register classes.
#define GET_REGINFO_ENUM
#include "Fox32GenRegisterInfo.inc"

#define GET_INSTRINFO_ENUM
#include "Fox32GenInstrInfo.inc"

#endif
