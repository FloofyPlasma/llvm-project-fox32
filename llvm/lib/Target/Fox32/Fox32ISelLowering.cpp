//===- NovaIselLowering.cpp - Nova DAG Lowering Implementation -----------===//
#include "Fox32ISelLowering.h"
#include "Fox32Subtarget.h"
#include "MCTargetDesc/Fox32MCTargetDesc.h"

using namespace llvm;

#define DEBUG_TYPE "fox32-isel"

Fox32TargetLowering::Fox32TargetLowering(const TargetMachine &TM,
                                         const Fox32Subtarget &STI)
    : TargetLowering(TM) {
  addRegisterClass(MVT::i32, &Fox32::GPR32RegClass);

  computeRegisterProperties(STI.getRegisterInfo());
}

SDValue
Fox32TargetLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv,
                                 bool isVarArg,
                                 const SmallVectorImpl<ISD::OutputArg> &Outs,
                                 const SmallVectorImpl<SDValue> &OutVals,
                                 const SDLoc &Dl, SelectionDAG &DAG) const {
  // Handle only integer return values
  // we need to copy the value to the r0 register.
  if (Outs.size() > 1) {
    reportFatalUsageError(
        "Multiple return values not supported\n"
        "This could be because the return type is a struct or a large integer"
        "that got split into multiple registers");
  }

  if (Outs.size() == 0) {
    return DAG.getNode(Fox32ISD::Ret, Dl, MVT::Other, Chain);
  }

  SDValue Glue;
  SmallVector<SDValue, 3> RetOps(1, Chain);
  for (unsigned I = 0, E = Outs.size(); I != E; ++I) {
    const ISD::OutputArg &Out = Outs[I];
    const SDValue &OutVal = OutVals[I];
    if (!Out.ArgVT.isScalarInteger() || Out.ArgVT.getScalarSizeInBits() > 32) {
      reportFatalUsageError("Only i32 return values are supported");
    }
    Chain = DAG.getCopyToReg(Chain, Dl, Fox32::r0, OutVal, Glue);
    Glue = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(Fox32::r0, Out.VT));
  }
  RetOps[0] = Chain;
  RetOps.push_back(Glue);

  return DAG.getNode(Fox32ISD::Ret, Dl, MVT::Other, RetOps);
}

SDValue Fox32TargetLowering::LowerCall(TargetLowering::CallLoweringInfo &CLI,
                                       SmallVectorImpl<SDValue> &InVals) const {
  return SDValue();
}

bool Fox32TargetLowering::CanLowerReturn(
    CallingConv::ID CallConv, MachineFunction &MF, bool IsVarArg,
    const SmallVectorImpl<ISD::OutputArg> &Outs, LLVMContext &Context,
    const Type *RetTy) const {
  return true;
}

const char *Fox32TargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch (Opcode) {
  case Fox32ISD::Ret:
    return "Fox32ISD::Ret";
  default:
    return "Unknown Fox32ISD::Node";
  }
}
