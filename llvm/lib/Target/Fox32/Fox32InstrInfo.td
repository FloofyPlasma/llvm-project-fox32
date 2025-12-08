//===- Fox32 Instruction Definitions ----------------------------===//
include "Fox32InstrFormats.td"

//==---------- All SD nodes for Fox32 ------------------===//
def Fox32RetSDN : SDNode<"Fox32ISD::Ret",
                        SDTNone, // 0 results and 0 operands
                        [SDNPHasChain, SDNPVariadic, SDNPOptInGlue]>;
//==---------- End SD Node definitions ----------------===//

//==----------- Fox32 Instruction Definitions ----------===//
def PseudoRet : PseudoFox32Inst<(outs), (ins), "ret"> {
  let isReturn = 1;
  let isTerminator = 1;
}
//==--------- End Fox32 Instruction Definitions --------===//

//==---- All patterns to match SD nodes -----------==//
def : Pat<(Fox32RetSDN), (PseudoRet)>;

def callseq_end : SDNode<"ISD::CALLSEQ_END", SDTNone, [SDNPHasChain, SDNPOptInGlue]>;

def ADJCALLSTACKDOWN : Instruction {
  let OutOperandList = (outs);
  let Namespace = "Fox32";
  let InOperandList = (ins);
  let AsmString = "ADJCALLSTACKDOWN";
  let Pattern = [(callseq_end)];
}

def ADJCALLSTACKUP : Instruction {
  let OutOperandList = (outs);
  let Namespace = "Fox32";
  let InOperandList = (ins);
  let AsmString = "ADJCALLSTACKUP";
  let Pattern = [(callseq_end)];
}
