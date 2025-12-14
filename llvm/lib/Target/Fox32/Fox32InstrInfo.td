//===- Fox32 Instruction Definitions ----------------------------===//
include "Fox32InstrFormats.td"

//==---------- All SD nodes for Fox32 ------------------===//
def Fox32RetSDN : SDNode<"Fox32ISD::Ret",
                         SDTNone, // 0 results and 0 operands
                         [SDNPHasChain, SDNPVariadic, SDNPOptInGlue]>;

def SDT_Fox32CallSeqStart
    : SDCallSeqStart<[SDTCisVT<0, i32>, SDTCisVT<1, i32>]>;
def SDT_Fox32CallSeqEnd : SDCallSeqEnd<[SDTCisVT<0, i32>, SDTCisVT<1, i32>]>;

def SDT_Fox32Call : SDTypeProfile<0, -1, [SDTCisVT<0, iPTR>]>;
def Fox32Call
    : SDNode<"Fox32ISD::Call", SDT_Fox32Call,
             [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue, SDNPVariadic]>;

// SD Node definitions for branching
def SDT_Fox32CmpICC : SDTypeProfile<0, 2, [SDTCisSameAs<0, 1>]>;
def Fox32CmpICC : SDNode<"Fox32ISD::CMPICC", SDT_Fox32CmpICC, [SDNPOutGlue]>;

def SDT_Fox32BrCC
    : SDTypeProfile<0, 2, [SDTCisVT<0, OtherVT>, SDTCisVT<1, i32>]>;
def Fox32BrCC
    : SDNode<"Fox32ISD::BR_CC", SDT_Fox32BrCC, [SDNPHasChain, SDNPInGlue]>;

def SDT_Fox32CMOV
    : SDTypeProfile<
          1, 3, [SDTCisSameAs<0, 1>, SDTCisSameAs<0, 2>, SDTCisVT<3, i32>]>;

def Fox32CMOV : SDNode<"Fox32ISD::CMOV", SDT_Fox32CMOV, [SDNPInGlue]>;

//==---------- End SD Node definitions ----------------===//

def addr : ComplexPattern<iPTR, 2, "SelectAddr", [frameindex], []>;

def calltarget : Operand<iPTR> {
  let EncoderMethod = "getCallTargetOpValue";
  let DecoderMethod = "decodeCallTarget";
  let PrintMethod = "printCallOperand";
}

def brtarget : Operand<OtherVT> {
  let EncoderMethod = "getBranchTargetOpValue";
  let OperandType = "OPERAND_PCREL";
  let PrintMethod = "printBranchOperand";
}

//==----------- Fox32 Instruction Definitions ----------===//
defm MOV : Mov<"mov">;

defm ADD : BinOp<"add", add>;
defm SUB : BinOp<"sub", sub>;
defm MUL : BinOp<"mul", mul>;
defm IMUL : BinOp<"imul", mul>; // TODO: this needs custom lowering for
                                // signed vs unsigned
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

defm LOAD_REGIMM : LoadRegImm<"mov">;
defm STORE_REGIMM : StoreRegImm<"mov">;

defm LOAD_REGREG : LoadRegReg<"mov">;
defm STORE_REGREG : StoreRegReg<"mov">;

defm PUSH : Push<"push">;
defm POP : Pop<"pop">;

let isReturn = 1, isTerminator = 1, isBarrier = 1 in {
  def RET : Fox32Inst<(outs), (ins), "ret"> {
    let Uses = [rsp];
    let Defs = [rsp];
  }
}

// Unconditional branch
let isBranch = 1, isTerminator = 1, isBarrier = 1 in {
  def JMP : Fox32Inst<(outs), (ins brtarget:$dst), "jmp $dst">;
}

// Conditional branches
let isBranch = 1, isTerminator = 1 in {
  def JMP_IFZ : Fox32Inst<(outs), (ins brtarget:$dst), "ifz jmp $dst">;

  def JMP_IFNZ : Fox32Inst<(outs), (ins brtarget:$dst), "ifnz jmp $dst">;

  def JMP_IFC : Fox32Inst<(outs), (ins brtarget:$dst), "ifc jmp $dst">;

  def JMP_IFNC : Fox32Inst<(outs), (ins brtarget:$dst), "ifnc jmp $dst">;

  def JMP_IFGT : Fox32Inst<(outs), (ins brtarget:$dst), "ifgt jmp $dst">;

  def JMP_IFLTEQ : Fox32Inst<(outs), (ins brtarget:$dst), "iflteq jmp $dst">;
}

let isCall = 1, Defs = [rsp, r0], Uses = [rsp] in {
  def CALL_32i
      : Fox32Inst<(outs), (ins calltarget:$dst, variable_ops), "call $dst">;
  def CALL_32r : Fox32Inst<(outs), (ins GPR32:$dst, variable_ops), "call $dst">;
}

let Constraints = "$src = $dst" in {
  def CMOV_IFZ_32 : Fox32Inst<(outs GPR32:$dst), (ins GPR32:$src, GPR32:$val),
                              "ifz mov $dst, $val">;
  def CMOV_IFNZ_32 : Fox32Inst<(outs GPR32:$dst), (ins GPR32:$src, GPR32:$val),
                               "ifnz mov $dst, $val">;
  def CMOV_IFC_32 : Fox32Inst<(outs GPR32:$dst), (ins GPR32:$src, GPR32:$val),
                              "ifc mov $dst, $val">;
  def CMOV_IFNC_32 : Fox32Inst<(outs GPR32:$dst), (ins GPR32:$src, GPR32:$val),
                               "ifnc mov $dst, $val">;
  def CMOV_IFGT_32 : Fox32Inst<(outs GPR32:$dst), (ins GPR32:$src, GPR32:$val),
                               "ifgt mov $dst, $val">;
  def CMOV_IFLTEQ_32
      : Fox32Inst<(outs GPR32:$dst), (ins GPR32:$src, GPR32:$val),
                  "iflteq mov $dst, $val">;
}

// Materialize frame index as address in register
let isPseudo = 1, isCodeGenOnly = 1 in {
  def LEA_FrameIndex
      : Fox32Inst<(outs GPR32:$dst), (ins MEMri:$addr), "# LEA $dst, $addr">;
}

//==--------- End Fox32 Instruction Definitions --------===//

//==---- All patterns to match SD nodes -----------==//

def : Pat<(Fox32RetSDN), (RET)>;

def : Pat<(Fox32Call tglobaladdr:$dst), (CALL_32i tglobaladdr:$dst)>;
def : Pat<(Fox32Call texternalsym:$dst), (CALL_32i texternalsym:$dst)>;
def : Pat<(Fox32Call GPR32:$dst), (CALL_32r GPR32:$dst)>;

def : Pat<(i8(load(add GPR32:$base, (i32 imm:$offset)))),
          (LOAD_REGIMM_8rri GPR32:$base, imm:$offset)>;
def : Pat<(i16(load(add GPR32:$base, (i32 imm:$offset)))),
          (LOAD_REGIMM_16rri GPR32:$base, imm:$offset)>;
def : Pat<(i32(load(add GPR32:$base, (i32 imm:$offset)))),
          (LOAD_REGIMM_32rri GPR32:$base, imm:$offset)>;

def : Pat<(store i8:$src, (add GPR32:$base, (i32 imm:$offset))),
          (STORE_REGIMM_8rri GPR32:$base, imm:$offset, GPR8:$src)>;
def : Pat<(store i16:$src, (add GPR32:$base, (i32 imm:$offset))),
          (STORE_REGIMM_16rri GPR32:$base, imm:$offset, GPR16:$src)>;
def : Pat<(store i32:$src, (add GPR32:$base, (i32 imm:$offset))),
          (STORE_REGIMM_32rri GPR32:$base, imm:$offset, GPR32:$src)>;

def : Pat<(i8(load(add GPR32:$base, GPR32:$index))),
          (LOAD_REGREG_8rrr GPR32:$base, GPR32:$index)>;
def : Pat<(i16(load(add GPR32:$base, GPR32:$index))),
          (LOAD_REGREG_16rrr GPR32:$base, GPR32:$index)>;
def : Pat<(i32(load(add GPR32:$base, GPR32:$index))),
          (LOAD_REGREG_32rrr GPR32:$base, GPR32:$index)>;

// Patterns for loading with complex addressing (handles frameindex)
def : Pat<(i32(load addr:$src)), (LOAD_REGIMM_32rri addr:$src)>;

def : Pat<(i16(load addr:$src)), (LOAD_REGIMM_16rri addr:$src)>;

def : Pat<(i8(load addr:$src)), (LOAD_REGIMM_8rri addr:$src)>;

// Patterns for storing with complex addressing (handles frameindex)
def : Pat<(store GPR32:$val, addr:$dst), (STORE_REGIMM_32rri addr:$dst,
                                             GPR32:$val)>;

def : Pat<(store GPR16:$val, addr:$dst), (STORE_REGIMM_16rri addr:$dst,
                                             GPR16:$val)>;

def : Pat<(store GPR8:$val, addr:$dst), (STORE_REGIMM_8rri addr:$dst,
                                            GPR8:$val)>;

// Pattern to select frame index address materialization
def : Pat<(i32 tframeindex:$fi), (LEA_FrameIndex tframeindex:$fi, (i32 0))>;

// Patterns for storing constants with complex addressing
// def : Pat<(store(i32 imm:$val), addr:$dst), (STORE_REGIMM_32rri addr:$dst,
//                                                 (MOV_32ri imm:$val))>;

// def : Pat<(store(i16 imm:$val), addr:$dst), (STORE_REGIMM_16rri addr:$dst,
//                                                 (MOV_16ri imm:$val))>;

// def : Pat<(store(i8 imm:$val), addr:$dst), (STORE_REGIMM_8rri addr:$dst,
//                                                (MOV_8ri imm:$val))>;

//==---------- Branch Patterns ------------------===//

// Unconditional branch
def : Pat<(br bb:$dst), (JMP bb:$dst)>;

// Patterns for CMP instruction to set flags
def : Pat<(Fox32CmpICC GPR32:$lhs, GPR32:$rhs), (CMP_32rr GPR32:$lhs,
                                                    GPR32:$rhs)>;

def : Pat<(Fox32CmpICC GPR32:$lhs, imm:$rhs), (CMP_32ri GPR32:$lhs, imm:$rhs)>;

// Map BR_CC with different condition codes to conditional jumps
def : Pat<(Fox32BrCC bb:$dst, 1), (JMP_IFZ bb:$dst)>;
def : Pat<(Fox32BrCC bb:$dst, 2), (JMP_IFNZ bb:$dst)>;
def : Pat<(Fox32BrCC bb:$dst, 3), (JMP_IFC bb:$dst)>;
def : Pat<(Fox32BrCC bb:$dst, 4), (JMP_IFNC bb:$dst)>;
def : Pat<(Fox32BrCC bb:$dst, 5), (JMP_IFGT bb:$dst)>;
def : Pat<(Fox32BrCC bb:$dst, 6), (JMP_IFLTEQ bb:$dst)>;

def : Pat<(Fox32CMOV GPR32:$false, GPR32:$true, (i32 1)),
          (CMOV_IFZ_32 GPR32:$false, GPR32:$true)>;
def : Pat<(Fox32CMOV GPR32:$false, GPR32:$true, (i32 2)),
          (CMOV_IFNZ_32 GPR32:$false, GPR32:$true)>;
def : Pat<(Fox32CMOV GPR32:$false, GPR32:$true, (i32 3)),
          (CMOV_IFC_32 GPR32:$false, GPR32:$true)>;
def : Pat<(Fox32CMOV GPR32:$false, GPR32:$true, (i32 4)),
          (CMOV_IFNC_32 GPR32:$false, GPR32:$true)>;
def : Pat<(Fox32CMOV GPR32:$false, GPR32:$true, (i32 5)),
          (CMOV_IFGT_32 GPR32:$false, GPR32:$true)>;
def : Pat<(Fox32CMOV GPR32:$false, GPR32:$true, (i32 6)),
          (CMOV_IFLTEQ_32 GPR32:$false, GPR32:$true)>;

//==---------- Call Sequence Patterns ------------------===//

def callseq_start : SDNode<"ISD::CALLSEQ_START",
                           SDT_Fox32CallSeqStart, [SDNPHasChain, SDNPOutGlue]>;
def callseq_end
    : SDNode<"ISD::CALLSEQ_END",
             SDT_Fox32CallSeqEnd, [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;

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
