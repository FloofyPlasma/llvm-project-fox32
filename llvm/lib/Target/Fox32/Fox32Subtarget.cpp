#include "Fox32Subtarget.h"
#include "Fox32.h"
#include "Fox32RegisterInfo.h"
#include "Fox32TargetMachine.h"

using namespace llvm;

#define DEBUG_TYPE "fox32-subtarget"

#define GET_SUBTARGETINFO_CTOR
#define GET_SUBTARGETINFO_TARGET_DESC
#include "Fox32GenSubtargetInfo.inc"
