#ifndef LLVM_LIB_TARGET_FOX32_FOX32ISELDAGTODAG_H
#define LLVM_LIB_TARGET_FOX32_FOX32ISELDAGTODAG_H

#include "Fox32Subtarget.h"
#include "Fox32TargetMachine.h"

#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/SelectionDAGISel.h"

namespace llvm {
class FunctionPass;
class Fox32TargetMachine;
class PassRegistry;

FunctionPass *createFox32ISelDagLegacy(Fox32TargetMachine &TM,
                                       CodeGenOptLevel OptLevel);

void initializeFox32DAGToDAGISelLegacyPass(PassRegistry &);

class Fox32DAGToDagISel final : public SelectionDAGISel {
  const Fox32Subtarget *SubTarget;

public:
  explicit Fox32DAGToDagISel(Fox32TargetMachine &TM, CodeGenOptLevel OptLevel)
      : SelectionDAGISel(TM, OptLevel) {}

  bool runOnMachineFunction(MachineFunction &MF) override;

private:
#include "Fox32GenDAGISel.inc"

  void Select(SDNode *Node) override;
};
} // end namespace llvm

#endif
