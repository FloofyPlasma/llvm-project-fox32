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

INITIALIZE_PASS_BEGIN(Fox32DAGToDAGISelLegacy, DEBUG_TYPE,
                      "Fox32 DAG->DAG Instruction Selection", false, false)
INITIALIZE_PASS_END(Fox32DAGToDAGISelLegacy, DEBUG_TYPE,
                    "Fox32 DAG->DAG Instruction Selection", false, false)

FunctionPass *llvm::createFox32ISelDagLegacy(Fox32TargetMachine &TM,
                                             CodeGenOptLevel OptLevel) {
  return new Fox32DAGToDAGISelLegacy(TM, OptLevel);
}

bool Fox32DAGToDagISel::runOnMachineFunction(MachineFunction &MF) {
  SubTarget = &MF.getSubtarget<Fox32Subtarget>();
  return SelectionDAGISel::runOnMachineFunction(MF);
}

void Fox32DAGToDagISel::Select(SDNode *Node) {
  // If already selected, skip
  if (Node->isMachineOpcode()) {
    Node->setNodeId(-1);
    return;
  }

  // Try to select using tablegen-generated patterns
  SelectCode(Node);
}
