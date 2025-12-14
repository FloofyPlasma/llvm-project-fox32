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
  if (Node->isMachineOpcode()) {
    Node->setNodeId(-1);
    return;
  }

  SDLoc DL(Node);
  unsigned Opcode = Node->getOpcode();
  if (Opcode == ISD::FrameIndex) {
    FrameIndexSDNode *FIN = cast<FrameIndexSDNode>(Node);
    int FI = FIN->getIndex();

    MachineFunction *MF = &CurDAG->getMachineFunction();
    const Fox32Subtarget &ST = MF->getSubtarget<Fox32Subtarget>();
    const Fox32FrameLowering *TFL = ST.getFrameLowering();
    const MachineFrameInfo &MFI = MF->getFrameInfo();

    int64_t Offset = MFI.getObjectOffset(FI);
    if (Offset < 0) {
      Offset = -Offset;
    }

    unsigned FrameReg = TFL->hasFP(*MF) ? Fox32::rfp : Fox32::rsp;

    if (Offset == 0) {
      SDValue FrameRegVal = CurDAG->getRegister(FrameReg, MVT::i32);
      SDNode *MovNode =
          CurDAG->getMachineNode(Fox32::MOV_32rr, DL, MVT::i32, FrameRegVal);
      ReplaceNode(Node, MovNode);
    } else {
      SDValue FrameRegVal = CurDAG->getRegister(FrameReg, MVT::i32);
      SDNode *MovNode =
          CurDAG->getMachineNode(Fox32::MOV_32rr, DL, MVT::i32, FrameRegVal);

      SDValue MovVal = SDValue(MovNode, 0);
      SDValue OffsetVal = CurDAG->getTargetConstant(Offset, DL, MVT::i32);
      SDNode *AddNode = CurDAG->getMachineNode(Fox32::ADD_32ri, DL, MVT::i32,
                                               MovVal, OffsetVal);
      ReplaceNode(Node, AddNode);
    }
    return;
  }

  SelectCode(Node);
}

bool Fox32DAGToDagISel::SelectAddr(SDValue Addr, SDValue &Base,
                                   SDValue &Offset) {
  SDLoc DL(Addr);

  // If this is a FrameIndex, convert it to a TargetFrameIndex
  if (FrameIndexSDNode *FIN = dyn_cast<FrameIndexSDNode>(Addr)) {
    Base = CurDAG->getTargetFrameIndex(FIN->getIndex(), MVT::i32);
    Offset = CurDAG->getTargetConstant(0, DL, MVT::i32);
    return true;
  }

  // Handle ADD with immediate offset (base + offset)
  if (Addr.getOpcode() == ISD::ADD) {
    // Check if right operand is a constant
    if (ConstantSDNode *CN = dyn_cast<ConstantSDNode>(Addr.getOperand(1))) {
      // Check if left operand is a FrameIndex
      if (FrameIndexSDNode *FIN =
              dyn_cast<FrameIndexSDNode>(Addr.getOperand(0))) {
        Base = CurDAG->getTargetFrameIndex(FIN->getIndex(), MVT::i32);
        Offset = CurDAG->getTargetConstant(CN->getSExtValue(), DL, MVT::i32);
        return true;
      }

      // Regular register + offset
      Base = Addr.getOperand(0);
      Offset = CurDAG->getTargetConstant(CN->getSExtValue(), DL, MVT::i32);
      return true;
    }
  }

  // Default: base with 0 offset
  Base = Addr;
  Offset = CurDAG->getTargetConstant(0, DL, MVT::i32);
  return true;
}
