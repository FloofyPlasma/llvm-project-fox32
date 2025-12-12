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

//==--------- End Fox32 Instruction Definitions --------===//

//==---- All patterns to match SD nodes -----------==//
def : Pat<(Fox32RetSDN), (PseudoRet)>;

def callseq_start
    : SDNode<"ISD::CALLSEQ_START", SDTNone, [SDNPHasChain, SDNPOutGlue]>;

def callseq_end : SDNode<"ISD::CALLSEQ_END",
                         SDTNone, [SDNPHasChain, SDNPInGlue, SDNPOutGlue]>;

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
