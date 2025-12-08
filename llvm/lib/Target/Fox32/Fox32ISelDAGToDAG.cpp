#include "Fox32ISelDAGToDAG.h"
#include "Fox32Subtarget.h"
#include "Fox32TargetMachine.h"

#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/Pass.h"
#include "llvm/Support/CodeGen.h"

using namespace llvm;

#define DEBUG_TYPE "fox32-isel"

namespace {
class Fox32DAGToDAGISelLegacy : public SelectionDAGISelLegacy {
public:
  static char ID;
  Fox32DAGToDAGISelLegacy(Fox32TargetMachine &TM, CodeGenOptLevel OptLevel)
      : SelectionDAGISelLegacy(
            ID, std::make_unique<Fox32DAGToDagISel>(TM, OptLevel)) {}
};
} // namespace

char Fox32DAGToDAGISelLegacy::ID = 0;

INITIALIZE_PASS(Fox32DAGToDAGISelLegacy, DEBUG_TYPE, "Fox32-isel", false,
                false);

FunctionPass *llvm::createFox32ISelDagLegacy(Fox32TargetMachine &TM,
                                             CodeGenOptLevel OptLevel) {
  return new Fox32DAGToDAGISelLegacy(TM, OptLevel);
}

bool Fox32DAGToDagISel::runOnMachineFunction(MachineFunction &MF) {
  SubTarget =
      &static_cast<const Fox32Subtarget &>(MF.getSubtarget<Fox32Subtarget>());
  return SelectionDAGISel::runOnMachineFunction(MF);
}

void Fox32DAGToDagISel::Select(SDNode *Node) {
  // Implement the selection logic here.
  // This is where you would match the SelectionDAG nodes to the target
  // instructions. For example, you might want to match a specific node type and
  // then create a corresponding machine instruction.

  // Example: if (Node->getOpcode() == ISD::ADD) { ... }
  // This is just a placeholder for the actual implementation.
  SelectCode(Node);
}
