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
  let isBarrier = 1;
  let Uses = [r0];
}

def MOVri : Fox32Inst<(outs GPR32:$dst), (ins i32imm:$src), "mov $dst, $src"> {
    let Pattern = [(set i32:$dst, imm:$src)];
}

def ADDrr : Fox32Inst<(outs GPR32:$dst), (ins GPR32: $dst_in, GPR32:$src), "add $dst, $src"> {
    let Pattern = [(set i32:$dst, (add i32:$dst_in, i32:$src))];
    let Constraints = "$dst_in = $dst";
}
//==--------- End Fox32 Instruction Definitions --------===//

//==---- All patterns to match SD nodes -----------==//
def : Pat<(Fox32RetSDN), (PseudoRet)>;

def callseq_start : SDNode<"ISD::CALLSEQ_START", SDTNone, [SDNPHasChain, SDNPOutGlue]>;

def callseq_end : SDNode<"ISD::CALLSEQ_END", SDTNone, [SDNPHasChain, SDNPInGlue, SDNPOutGlue]>;

def ADJCALLSTACKDOWN : Instruction {
  let OutOperandList = (outs);
  let InOperandList = (ins);
  let Namespace = "Fox32";
  let AsmString = "";
  let Pattern = [(callseq_start)];
  let isCodeGenOnly = 1;
  let hasSideEffects = 1;
}

def ADJCALLSTACKUP : Instruction {
  let OutOperandList = (outs);
  let InOperandList = (ins);
  let Namespace = "Fox32";
  let AsmString = "";
  let Pattern = [(callseq_end)];
  let isCodeGenOnly = 1;
  let hasSideEffects = 1;
}
