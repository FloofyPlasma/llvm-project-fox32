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
#include "llvm/CodeGen/TargetLowering.h"
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

  setOperationAction(ISD::BR_CC, MVT::i32, Custom);
  setOperationAction(ISD::BRCOND, MVT::Other, Custom);
  setOperationAction(ISD::SETCC, MVT::i32, Custom);
  setOperationAction(ISD::SELECT_CC, MVT::i32, Custom);

  setOperationAction(ISD::SELECT, MVT::i32, Custom);

  setBooleanContents(ZeroOrOneBooleanContent);

  setOperationAction(ISD::FrameIndex, MVT::i32, Custom);

  // Fox32 doesn't have instructions that return both high and low parts
  // Tell LLVM to expand these into separate operations
  setOperationAction(ISD::SMUL_LOHI, MVT::i32, Expand);
  setOperationAction(ISD::UMUL_LOHI, MVT::i32, Expand);
  setOperationAction(ISD::SMUL_LOHI, MVT::i16, Expand);
  setOperationAction(ISD::UMUL_LOHI, MVT::i16, Expand);
  setOperationAction(ISD::SMUL_LOHI, MVT::i8, Expand);
  setOperationAction(ISD::UMUL_LOHI, MVT::i8, Expand);

  // Also expand MULHU and MULHS if they appear
  setOperationAction(ISD::MULHU, MVT::i32, Expand);
  setOperationAction(ISD::MULHS, MVT::i32, Expand);
  setOperationAction(ISD::MULHU, MVT::i16, Expand);
  setOperationAction(ISD::MULHS, MVT::i16, Expand);
  setOperationAction(ISD::MULHU, MVT::i8, Expand);
  setOperationAction(ISD::MULHS, MVT::i8, Expand);
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
  SelectionDAG &DAG = CLI.DAG;
  SDLoc &dl = CLI.DL;
  SmallVectorImpl<ISD::OutputArg> &Outs = CLI.Outs;
  SmallVectorImpl<SDValue> &OutVals = CLI.OutVals;
  SmallVectorImpl<ISD::InputArg> &Ins = CLI.Ins;
  SDValue Chain = CLI.Chain;
  SDValue Callee = CLI.Callee;
  CallingConv::ID CallConv = CLI.CallConv;
  bool isVarArg = CLI.IsVarArg;

  MachineFunction &MF = DAG.getMachineFunction();

  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, MF, ArgLocs, *DAG.getContext());
  CCInfo.AnalyzeCallOperands(Outs, CC_Fox32);

  unsigned NumBytes = CCInfo.getStackSize();

  Chain = DAG.getCALLSEQ_START(Chain, NumBytes, 0, dl);

  SmallVector<std::pair<unsigned, SDValue>, 8> RegsToPass;
  SmallVector<SDValue, 8> MemOpChains;

  // First pass: prepare all arguments (do extensions, etc.)
  SmallVector<SDValue, 8> PreparedArgs;
  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];
    SDValue Arg = OutVals[i];

    switch (VA.getLocInfo()) {
    default:
      llvm_unreachable("Unknown loc info!");
    case CCValAssign::Full:
      break;
    case CCValAssign::SExt:
      Arg = DAG.getNode(ISD::SIGN_EXTEND, dl, VA.getLocVT(), Arg);
      break;
    case CCValAssign::ZExt:
      Arg = DAG.getNode(ISD::ZERO_EXTEND, dl, VA.getLocVT(), Arg);
      break;
    case CCValAssign::AExt:
      Arg = DAG.getNode(ISD::ANY_EXTEND, dl, VA.getLocVT(), Arg);
      break;
    }

    // CRITICAL FIX: If this is a constant, create a new node for each use
    // This prevents the scheduler from getting confused by shared constant
    // nodes
    if (ConstantSDNode *C = dyn_cast<ConstantSDNode>(Arg)) {
      Arg = DAG.getConstant(C->getAPIntValue(), dl, Arg.getValueType());
    }

    PreparedArgs.push_back(Arg);
  }

  // Second pass: assign to registers or stack
  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];
    SDValue Arg = PreparedArgs[i];

    if (VA.isRegLoc()) {
      RegsToPass.push_back(std::make_pair(VA.getLocReg(), Arg));
    } else {
      assert(VA.isMemLoc());

      SDValue PtrOff =
          DAG.getIntPtrConstant(VA.getLocMemOffset(), dl, /*isTarget=*/true);
      SDValue StackPtr = DAG.getRegister(Fox32::rsp, MVT::i32);
      PtrOff = DAG.getNode(ISD::ADD, dl, MVT::i32, StackPtr, PtrOff);

      MemOpChains.push_back(
          DAG.getStore(Chain, dl, Arg, PtrOff, MachinePointerInfo()));
    }
  }

  if (!MemOpChains.empty()) {
    Chain = DAG.getNode(ISD::TokenFactor, dl, MVT::Other, MemOpChains);
  }

  // Build the glue chain for register arguments
  SDValue InGlue;
  for (unsigned i = 0, e = RegsToPass.size(); i != e; ++i) {
    Chain = DAG.getCopyToReg(Chain, dl, RegsToPass[i].first,
                             RegsToPass[i].second, InGlue);
    InGlue = Chain.getValue(1);
  }

  if (GlobalAddressSDNode *G = dyn_cast<GlobalAddressSDNode>(Callee))
    Callee = DAG.getTargetGlobalAddress(G->getGlobal(), dl, MVT::i32);
  else if (ExternalSymbolSDNode *E = dyn_cast<ExternalSymbolSDNode>(Callee))
    Callee = DAG.getTargetExternalSymbol(E->getSymbol(), MVT::i32);

  SmallVector<SDValue, 8> Ops;
  Ops.push_back(Chain);
  Ops.push_back(Callee);

  // Add argument registers
  for (unsigned i = 0, e = RegsToPass.size(); i != e; ++i)
    Ops.push_back(DAG.getRegister(RegsToPass[i].first,
                                  RegsToPass[i].second.getValueType()));

  const TargetRegisterInfo *TRI = MF.getSubtarget().getRegisterInfo();
  const uint32_t *Mask = TRI->getCallPreservedMask(MF, CallConv);
  assert(Mask && "Missing call preserved mask for calling convention");
  Ops.push_back(DAG.getRegisterMask(Mask));

  if (InGlue.getNode())
    Ops.push_back(InGlue);

  SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Glue);

  Chain = DAG.getNode(Fox32ISD::Call, dl, NodeTys, Ops);
  InGlue = Chain.getValue(1);

  Chain = DAG.getCALLSEQ_END(Chain, NumBytes, 0, InGlue, dl);
  InGlue = Chain.getValue(1);

  return LowerCallResult(Chain, InGlue, CallConv, isVarArg, Ins, dl, DAG,
                         InVals);
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
  case Fox32ISD::CMP:
    return "Fox32ISD::CMP";
  case Fox32ISD::CMPICC:
    return "Fox32ISD::CMPICC";
  case Fox32ISD::BR_CC:
    return "Fox32ISD::BR_CC";
  case Fox32ISD::CMOV:
    return "Fox32ISD::CMOV";
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

SDValue Fox32TargetLowering::LowerCallResult(
    SDValue Chain, SDValue InGlue, CallingConv::ID CallConv, bool isVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &dl,
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {

  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), RVLocs,
                 *DAG.getContext());

  CCInfo.AnalyzeCallResult(Ins, RetCC_Fox32);

  for (auto &VA : RVLocs) {
    Chain = DAG.getCopyFromReg(Chain, dl, VA.getLocReg(), VA.getValVT(), InGlue)
                .getValue(1);
    InGlue = Chain.getValue(2);
    InVals.push_back(Chain.getValue(0));
  }

  return Chain;
}

SDValue Fox32TargetLowering::LowerOperation(SDValue Op,
                                            SelectionDAG &DAG) const {
  switch (Op.getOpcode()) {
  case ISD::BR_CC:
    return LowerBR_CC(Op, DAG);
  case ISD::SETCC:
    return LowerSETCC(Op, DAG);
  case ISD::BRCOND:
    return LowerBRCOND(Op, DAG);
  case ISD::SELECT_CC:
    return LowerSELECT_CC(Op, DAG);
  case ISD::FrameIndex:
    return LowerFrameIndex(Op, DAG);
  }

  return TargetLowering::LowerOperation(Op, DAG);
}

static unsigned getCondCodeForSetCC(ISD::CondCode CC) {
  switch (CC) {
  default:
    llvm_unreachable("Unsupported condition code");
  case ISD::SETEQ:
    return 1; // ifz
  case ISD::SETNE:
    return 2; // ifnz
  case ISD::SETULT:
    return 3; // ifc (carry set = less than for unsigned)
  case ISD::SETUGE:
    return 4; // ifnc (carry clear = greater or equal for unsigned)
  case ISD::SETUGT:
    return 5; // ifgt (neither zero nor carry)
  case ISD::SETULE:
    return 6; // iflteq (zero or carry)
  // Signed comparisons (use ICMP instead of CMP)
  case ISD::SETLT:
    return 3; // ifc with ICMP
  case ISD::SETGE:
    return 4; // ifnc with ICMP
  case ISD::SETGT:
    return 5; // ifgt with ICMP
  case ISD::SETLE:
    return 6; // iflteq with ICMP
  }
}

static bool isSignedCC(ISD::CondCode CC) {
  switch (CC) {
  case ISD::SETLT:
  case ISD::SETLE:
  case ISD::SETGT:
  case ISD::SETGE:
    return true;
  default:
    return false;
  }
}

SDValue Fox32TargetLowering::LowerBR_CC(SDValue Op, SelectionDAG &DAG) const {
  SDValue Chain = Op.getOperand(0);
  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(1))->get();
  SDValue LHS = Op.getOperand(2);
  SDValue RHS = Op.getOperand(3);
  SDValue Dest = Op.getOperand(4);
  SDLoc dl(Op);

  SDValue Cmp = DAG.getNode(Fox32ISD::CMPICC, dl, MVT::Glue, LHS, RHS);

  unsigned CondCode = getCondCodeForSetCC(CC);
  SDValue CondVal = DAG.getConstant(CondCode, dl, MVT::i32);

  return DAG.getNode(Fox32ISD::BR_CC, dl, MVT::Other, Chain, Dest, CondVal,
                     Cmp);
}

SDValue Fox32TargetLowering::LowerBRCOND(SDValue Op, SelectionDAG &DAG) const {
  SDValue Chain = Op.getOperand(0);
  SDValue Cond = Op.getOperand(1);
  SDValue Dest = Op.getOperand(2);
  SDLoc dl(Op);

  if (Cond.getOpcode() == ISD::SETCC) {
    SDValue LHS = Cond.getOperand(0);
    SDValue RHS = Cond.getOperand(1);
    ISD::CondCode CC = cast<CondCodeSDNode>(Cond.getOperand(2))->get();

    SDValue Cmp = DAG.getNode(Fox32ISD::CMPICC, dl, MVT::Glue, LHS, RHS);

    unsigned CondCode = getCondCodeForSetCC(CC);
    SDValue CondVal = DAG.getConstant(CondCode, dl, MVT::i32);

    return DAG.getNode(Fox32ISD::BR_CC, dl, MVT::Other, Chain, Dest, CondVal,
                       Cmp);
  }

  SDValue Zero = DAG.getConstant(0, dl, Cond.getValueType());
  SDValue Cmp = DAG.getNode(Fox32ISD::CMPICC, dl, MVT::Glue, Cond, Zero);

  SDValue CondVal = DAG.getConstant(2, dl, MVT::i32); // ifnz
  return DAG.getNode(Fox32ISD::BR_CC, dl, MVT::Other, Chain, Dest, CondVal,
                     Cmp);
}

SDValue Fox32TargetLowering::LowerSETCC(SDValue Op, SelectionDAG &DAG) const {
  SDValue LHS = Op.getOperand(0);
  SDValue RHS = Op.getOperand(1);
  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(2))->get();
  SDLoc dl(Op);

  // Perform comparison to set flags
  SDValue Cmp = DAG.getNode(Fox32ISD::CMPICC, dl, MVT::Glue, LHS, RHS);

  unsigned CondCode = getCondCodeForSetCC(CC);

  SDValue Zero = DAG.getConstant(0, dl, MVT::i32);
  SDValue One = DAG.getConstant(1, dl, MVT::i32);
  SDValue CondVal = DAG.getConstant(CondCode, dl, MVT::i32);

  // Create a Fox32-specific node for conditional move
  // This will be pattern-matched to: ifCC mov result, 1
  return DAG.getNode(Fox32ISD::CMOV, dl, MVT::i32, Zero, One, CondVal, Cmp);
}

SDValue Fox32TargetLowering::LowerSELECT_CC(SDValue Op,
                                            SelectionDAG &DAG) const {
  SDValue LHS = Op.getOperand(0);
  SDValue RHS = Op.getOperand(1);
  SDValue TrueV = Op.getOperand(2);
  SDValue FalseV = Op.getOperand(3);
  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(4))->get();
  SDLoc dl(Op);

  // Check if this is a simple comparison resulting in 0 or 1
  if (isa<ConstantSDNode>(TrueV) && isa<ConstantSDNode>(FalseV)) {
    int64_t TrueVal = cast<ConstantSDNode>(TrueV)->getSExtValue();
    int64_t FalseVal = cast<ConstantSDNode>(FalseV)->getSExtValue();

    // Pattern: (x == y) ? 1 : 0  or  (x != y) ? 1 : 0
    if ((TrueVal == 1 && FalseVal == 0)) {
      // This is just SETCC, redirect to that
      return DAG.getSetCC(dl, Op.getValueType(), LHS, RHS, CC);
    }

    // Pattern: (x == y) ? 0 : 1
    if (TrueVal == 0 && FalseVal == 1) {
      // Invert the condition
      ISD::CondCode InvCC = ISD::getSetCCInverse(CC, LHS.getValueType());
      return DAG.getSetCC(dl, Op.getValueType(), LHS, RHS, InvCC);
    }
  }

  // For all other cases, use conditional move
  SDValue Cmp = DAG.getNode(Fox32ISD::CMPICC, dl, MVT::Glue, LHS, RHS);

  unsigned CondCode = getCondCodeForSetCC(CC);
  SDValue CondVal = DAG.getConstant(CondCode, dl, MVT::i32);

  // Use CMOV: if condition is true, use TrueV, otherwise use FalseV
  return DAG.getNode(Fox32ISD::CMOV, dl, Op.getValueType(), FalseV, TrueV,
                     CondVal, Cmp);
}

SDValue Fox32TargetLowering::LowerFrameIndex(SDValue Op,
                                             SelectionDAG &DAG) const {

  int FI = cast<FrameIndexSDNode>(Op)->getIndex();
  EVT VT = Op.getValueType();

  SDLoc DL(Op);

  return DAG.getTargetFrameIndex(FI, VT);
}
