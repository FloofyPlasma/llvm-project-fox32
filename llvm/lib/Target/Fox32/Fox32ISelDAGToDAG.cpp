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

bool Fox32DAGToDagISel::SelectAddr(SDValue Addr, SDValue &Base,
                                   SDValue &Offset) {
  // If the address is a FrameIndex, leave it to be matched by patterns
  if (FrameIndexSDNode *FIN = dyn_cast<FrameIndexSDNode>(Addr)) {
    Base = CurDAG->getTargetFrameIndex(FIN->getIndex(), MVT::i32);
    Offset = CurDAG->getTargetConstant(0, SDLoc(Addr), MVT::i32);
    return true;
  }

  // If it's an ADD node, try to extract base + offset
  if (Addr.getOpcode() == ISD::ADD) {
    if (ConstantSDNode *CN = dyn_cast<ConstantSDNode>(Addr.getOperand(1))) {
      Base = Addr.getOperand(0);
      Offset =
          CurDAG->getTargetConstant(CN->getSExtValue(), SDLoc(Addr), MVT::i32);
      return true;
    }
  }

  // Otherwise use the address as base with 0 offset
  Base = Addr;
  Offset = CurDAG->getTargetConstant(0, SDLoc(Addr), MVT::i32);
  return true;
}
