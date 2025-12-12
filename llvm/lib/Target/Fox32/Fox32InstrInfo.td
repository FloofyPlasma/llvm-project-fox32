//===- Fox32 Instruction Definitions ----------------------------===//
include "Fox32InstrFormats.td"

//==---------- All SD nodes for Fox32 ------------------===//
def Fox32RetSDN : SDNode<"Fox32ISD::Ret",
                         SDTNone, // 0 results and 0 operands
                         [SDNPHasChain, SDNPVariadic, SDNPOptInGlue]>;

def SDT_Fox32CallSeqStart : SDCallSeqStart<[SDTCisVT<0, i32>, SDTCisVT<1, i32>]>;
def SDT_Fox32CallSeqEnd   : SDCallSeqEnd<[SDTCisVT<0, i32>, SDTCisVT<1, i32>]>;


def SDT_Fox32Call : SDTypeProfile<0, -1, [SDTCisVT<0, iPTR>]>;
def Fox32Call : SDNode<"Fox32ISD::Call", SDT_Fox32Call,
                       [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue, SDNPVariadic]>;

//==---------- End SD Node definitions ----------------===//

//==----------- Fox32 Instruction Definitions ----------===//
defm MOV : Mov<"mov">;

defm ADD : BinOp<"add", add>;
defm SUB : BinOp<"sub", sub>;
defm MUL : BinOp<"mul", mul>;
defm IMUL : BinOp<"imul", mul>; // TODO: this needs custom lowering for signed
                                // vs unsigned
defm DIV : BinOp<"div", udiv>;
defm IDIV : BinOp<"idiv", sdiv>;
defm REM : BinOp<"rem", urem>;
defm IREM : BinOp<"irem", srem>;

defm AND : BinOp<"and", and>;
defm OR : BinOp<"or", or>;
defm XOR : BinOp<"xor", xor>;

defm SLA : ShiftOp<"sla", shl>;
defm SRA : ShiftOp<"sra", sra>;
defm SRL : ShiftOp<"srl", srl>;

// TODO: implement rotate operations
// defm ROL : ShiftOp<"rol", rotl>;
// defm ROR : ShiftOp<"ror", rotr>;

defm NOT : NotOp<"not">;
defm INC : IncOp<"inc">;
defm DEC : DecOp<"dec">;

defm CMP : Cmp<"cmp">;
defm ICMP : Cmp<"icmp">;

defm LOAD : Load<"mov">;
defm STORE : Store<"mov">;

defm PUSH : Push<"push">;
defm POP : Pop<"pop">;

let isReturn = 1, isTerminator = 1, isBarrier = 1 in {
  def RET : Fox32Inst<(outs), (ins), "ret"> {
    let Uses = [rsp, r0];
    let Defs = [rsp];
  }
}


let isCall = 1, Defs = [rsp, r0], Uses = [rsp] in {
  def CALL_32i : Fox32Inst<(outs), (ins i32imm:$dst, variable_ops), "call $dst">;
  def CALL_32r : Fox32Inst<(outs), (ins GPR32:$dst, variable_ops), "call $dst">;
}

//==--------- End Fox32 Instruction Definitions --------===//

//==---- All patterns to match SD nodes -----------==//

def : Pat<(Fox32RetSDN), (RET)>;

def : Pat<(Fox32Call tglobaladdr:$dst),
          (CALL_32i tglobaladdr:$dst)>;
def : Pat<(Fox32Call texternalsym:$dst),
          (CALL_32i texternalsym:$dst)>;
def : Pat<(Fox32Call GPR32:$dst),
          (CALL_32r GPR32:$dst)>;

def callseq_start : SDNode<"ISD::CALLSEQ_START", SDT_Fox32CallSeqStart,
                           [SDNPHasChain, SDNPOutGlue]>;
def callseq_end   : SDNode<"ISD::CALLSEQ_END", SDT_Fox32CallSeqEnd,
                           [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;

def ADJCALLSTACKDOWN : Instruction {
  let OutOperandList = (outs);
  let InOperandList = (ins i32imm:$amt1, i32imm:$amt2);
  let Namespace = "Fox32";
  let AsmString = "";
  let Pattern = [(callseq_start timm:$amt1, timm:$amt2)];
  let isCodeGenOnly = 1;
  let hasSideEffects = 1;
  let Defs = [rsp];
  let Uses = [rsp];
}

def ADJCALLSTACKUP : Instruction {
  let OutOperandList = (outs);
  let InOperandList = (ins i32imm:$amt1, i32imm:$amt2);
  let Namespace = "Fox32";
  let AsmString = "";
  let Pattern = [(callseq_end timm:$amt1, timm:$amt2)];
  let isCodeGenOnly = 1;
  let hasSideEffects = 1;
  let Defs = [rsp];
  let Uses = [rsp];
}
