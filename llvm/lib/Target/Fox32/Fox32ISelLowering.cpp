//===- Fox32IselLowering.cpp - Fox32 DAG Lowering Implementation
//-----------===//
#include "Fox32ISelLowering.h"
#include "Fox32Subtarget.h"
#include "MCTargetDesc/Fox32MCTargetDesc.h"

#include "llvm/CodeGen/CallingConvLower.h"

using namespace llvm;

namespace {
static bool CC_Fox32(unsigned ValNo, MVT ValVt, MVT LocVT,
                     CCValAssign::LocInfo LocInfo, ISD::ArgFlagsTy ArgFlags,
                     Type *OrigTy, // <-- Add this parameter
                     CCState &State) {
  static const MCPhysReg IntArgRegs[] = {
      Fox32::r0, Fox32::r1, Fox32::r2, Fox32::r3,
      Fox32::r4, Fox32::r5, Fox32::r6, Fox32::r7,
  };

  if (ValVt != MVT::i32) {
    return true; // type not supported
  }

  if (unsigned Reg = State.AllocateReg(IntArgRegs)) {
    State.addLoc(CCValAssign::getReg(ValNo, ValVt, Reg, LocVT, LocInfo));
    return false;
  }

  llvm_unreachable("Stack arguments not implemented");
}
} // end anonymous namespace

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

SDValue Fox32TargetLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool isVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &dl,
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {
  MachineFunction &MF = DAG.getMachineFunction();
  MachineRegisterInfo &RegInfo = MF.getRegInfo();

  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, MF, ArgLocs, *DAG.getContext());
  CCInfo.AnalyzeFormalArguments(Ins, CC_Fox32);

  for (unsigned i = 0; i != ArgLocs.size(); ++i) {
    CCValAssign &VA = ArgLocs[i];
    if (VA.isRegLoc()) {
      Register Reg = VA.getLocReg();

      // Mark the register as live-in
      if (!RegInfo.isLiveIn(Reg)) {
        RegInfo.addLiveIn(Reg);
      }

      Register VReg = RegInfo.createVirtualRegister(&Fox32::GPR32RegClass);
      RegInfo.addLiveIn(Reg, VReg);
      SDValue ArgValue = DAG.getCopyFromReg(Chain, dl, VReg, VA.getValVT());

      InVals.push_back(ArgValue);
    } else {
      llvm_unreachable("Stack arguments not implemented");
    }
  }

  return Chain;
}
