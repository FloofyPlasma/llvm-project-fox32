//===- Fox32IselLowering.cpp - Fox32 DAG Lowering Implementation
//-----------===//
#include "Fox32ISelLowering.h"
#include "Fox32RegisterInfo.h"
#include "Fox32Subtarget.h"
#include "MCTargetDesc/Fox32MCTargetDesc.h"

#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/Support/Debug.h"

using namespace llvm;

#define DEBUG_TYPE "fox32-isel"

#include "Fox32GenCallingConv.inc"

Fox32TargetLowering::Fox32TargetLowering(const TargetMachine &TM,
                                         const Fox32Subtarget &STI)
    : TargetLowering(TM) {
  addRegisterClass(MVT::i8, &Fox32::GPR8RegClass);
  addRegisterClass(MVT::i16, &Fox32::GPR16RegClass);
  addRegisterClass(MVT::i32, &Fox32::GPR32RegClass);

  computeRegisterProperties(STI.getRegisterInfo());

  setStackPointerRegisterToSaveRestore(Fox32::rsp);
}

SDValue
Fox32TargetLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv,
                                 bool isVarArg,
                                 const SmallVectorImpl<ISD::OutputArg> &Outs,
                                 const SmallVectorImpl<SDValue> &OutVals,
                                 const SDLoc &dl, SelectionDAG &DAG) const {
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), RVLocs,
                 *DAG.getContext());
  CCInfo.AnalyzeReturn(Outs, RetCC_Fox32);

  SDValue Flag;
  SmallVector<SDValue, 4> RetOps(1, Chain);

  // Copy return values to registers
  for (unsigned i = 0; i != RVLocs.size(); ++i) {
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");

    Chain = DAG.getCopyToReg(Chain, dl, VA.getLocReg(), OutVals[i], Flag);
    Flag = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
  }

  RetOps[0] = Chain;
  if (Flag.getNode()) {
    RetOps.push_back(Flag);
  }

  return DAG.getNode(Fox32ISD::Ret, dl, MVT::Other, RetOps);
}

SDValue Fox32TargetLowering::LowerCall(TargetLowering::CallLoweringInfo &CLI,
                                       SmallVectorImpl<SDValue> &InVals) const {
  // TODO: Implement call lowering
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
  case Fox32ISD::Call:
    return "Fox32ISD::Call";
  default:
    return "Unknown Fox32ISD::Node";
  }
}

SDValue Fox32TargetLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool isVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &dl,
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {
  MachineFunction &MF = DAG.getMachineFunction();
  MachineFrameInfo &MFI = MF.getFrameInfo();
  MachineRegisterInfo &RegInfo = MF.getRegInfo();

  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, MF, ArgLocs, *DAG.getContext());
  CCInfo.AnalyzeFormalArguments(Ins, CC_Fox32);

  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];
    if (VA.isRegLoc()) {
      // Argument passed in register
      EVT RegVT = VA.getLocVT();
      const TargetRegisterClass *RC;

      if (RegVT == MVT::i8) {
        RC = &Fox32::GPR8RegClass;
      } else if (RegVT == MVT::i16) {
        RC = &Fox32::GPR16RegClass;
      } else if (RegVT == MVT::i32) {
        RC = &Fox32::GPR32RegClass;
      } else {
        llvm_unreachable("Unsupported argument type");
      }

      Register VReg = MF.getRegInfo().createVirtualRegister(RC);
      MF.getRegInfo().addLiveIn(VA.getLocReg(), VReg);
      SDValue ArgValue = DAG.getCopyFromReg(Chain, dl, VReg, RegVT);

      InVals.push_back(ArgValue);
    } else {
      // Argument passed on stack
      assert(VA.isMemLoc());
      int FI = MFI.CreateFixedObject(VA.getValVT().getSizeInBits() / 8,
                                     VA.getLocMemOffset(), true);
      SDValue FIPtr = DAG.getFrameIndex(FI, getPointerTy(DAG.getDataLayout()));
      SDValue Load = DAG.getLoad(VA.getValVT(), dl, Chain, FIPtr,
                                 MachinePointerInfo::getFixedStack(MF, FI));

      InVals.push_back(Load);
    }
  }

  return Chain;
}
