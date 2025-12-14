#include "Fox32ISelDAGToDAG.h"
#include "Fox32Subtarget.h"
#include "Fox32TargetMachine.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
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
  SDLoc DL(Addr);
  MachineFunction *MF = &CurDAG->getMachineFunction();
  const Fox32Subtarget &ST = MF->getSubtarget<Fox32Subtarget>();
  const Fox32FrameLowering *TFL = ST.getFrameLowering();
  MachineFrameInfo &MFI = MF->getFrameInfo();

  if (auto *FI = dyn_cast<FrameIndexSDNode>(Addr)) {
    int FIOffset = MFI.getObjectOffset(FI->getIndex());
    bool HasFP = TFL->hasFP(*MF);

    Base = CurDAG->getRegister(HasFP ? Fox32::rfp : Fox32::rsp, MVT::i32);
    Offset = CurDAG->getTargetConstant(
        FIOffset + (HasFP ? 0 : MFI.getStackSize()), DL, MVT::i32);
    return true;
  }

  if (Addr.getOpcode() == ISD::ADD) {
    if (auto *CN = dyn_cast<ConstantSDNode>(Addr.getOperand(1))) {
      Base = Addr.getOperand(0);
      Offset = CurDAG->getTargetConstant(CN->getSExtValue(), DL, MVT::i32);
      return true;
    }
  }

  Base = Addr;
  Offset = CurDAG->getTargetConstant(0, DL, MVT::i32);
  return true;
}
