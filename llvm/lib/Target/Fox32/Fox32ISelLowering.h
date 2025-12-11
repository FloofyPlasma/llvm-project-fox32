//==-- Fox32 DAG Lowering Interface --------//

#ifndef LLVM_LIB_TARGET_FOX32_FOX32ISELLOWERING_H
#define LLVM_LIB_TARGET_FOX32_FOX32ISELLOWERING_H

#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/ISDOpcodes.h"
#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {

namespace Fox32ISD {
enum NodeType : unsigned {
  FIRST_NUMBER = ISD::BUILTIN_OP_END,

  // Return
  Ret,
};
} // end namespace Fox32ISD

class Fox32Subtarget;

class Fox32TargetLowering : public TargetLowering {
public:
  explicit Fox32TargetLowering(const TargetMachine &TM,
                               const Fox32Subtarget &STI);

  SDValue LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool isVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &Outs,
                      const SmallVectorImpl<SDValue> &OutVals, const SDLoc &dl,
                      SelectionDAG &DAG) const override;

  SDValue LowerCall(TargetLowering::CallLoweringInfo &CLI,
                    SmallVectorImpl<SDValue> &InVals) const override;

  bool CanLowerReturn(CallingConv::ID CallConv, MachineFunction &MF,
                      bool IsVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &Outs,
                      LLVMContext &Context, const Type *RetTy) const override;
  SDValue
  LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv,
                       bool isVarArg,
                       const SmallVectorImpl<ISD::InputArg> & Ins,
                       const SDLoc & dl, SelectionDAG & DAG,
                       SmallVectorImpl<SDValue> & InVals) const override;
  /// getTargetNodeName - This method returns the name of a target specific
  //  DAG node.
  const char *getTargetNodeName(unsigned Opcode) const override;
};

} // end namespace llvm

#endif
