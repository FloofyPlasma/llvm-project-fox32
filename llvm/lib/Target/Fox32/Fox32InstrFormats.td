//===-- Fox32InstrFormats.td - Fox32 Instruction Formats
//--------------------===//
// This file contains the instruction formats for the Fox32 architecture.
//===----------------------------------------------------------------------===//

class Fox32Inst<dag outs, dag ins, string asmString> : Instruction {
  let Namespace = "Fox32";
  let OutOperandList = outs;
  let InOperandList = ins;
  let AsmString = asmString;
}

class PseudoFox32Inst<dag outs, dag ins, string asmString>
    : Fox32Inst<outs, ins, asmString> {
  let isPseudo = 1;
  let isCodeGenOnly = 1;
}

def MEMri : Operand<iPTR> {
  let PrintMethod = "printMemRegImmOperand";
  let MIOperandInfo = (ops GPR32:$base, i32imm:$offset);
}

def MEMrr : Operand<iPTR> {
  let PrintMethod = "printMemRegRegOperand";
  let MIOperandInfo = (ops GPR32:$base, GPR32:$index);
}

//===----------------------------------------------------------------------===//
//  Multiclasses for Automatic Size Variant Generation
//===----------------------------------------------------------------------===//

// Multiclass for register-to-register binary operation (add, sub, mul, etc.)
multiclass BinOpRR<string opcode, SDNode node> {
  def _8rr : Fox32Inst<(outs GPR8:$dst), (ins GPR8:$dst_in, GPR8:$src),
                       !strconcat(opcode, ".8 $dst, $src")> {
    let Pattern = [(set i8:$dst, (node i8:$dst_in, i8:$src))];
    let Constraints = "$dst_in = $dst";
  }

  def _16rr : Fox32Inst<(outs GPR16:$dst), (ins GPR16:$dst_in, GPR16:$src),
                        !strconcat(opcode, ".16 $dst, $src")> {
    let Pattern = [(set i16:$dst, (node i16:$dst_in, i16:$src))];
    let Constraints = "$dst_in = $dst";
  }

  def _32rr : Fox32Inst<(outs GPR32:$dst), (ins GPR32:$dst_in, GPR32:$src),
                        !strconcat(opcode, ".32 $dst, $src")> {
    let Pattern = [(set i32:$dst, (node i32:$dst_in, i32:$src))];
    let Constraints = "$dst_in = $dst";
  }
}

// Multiclass for register-immediate binary operations
multiclass BinOpRI<string opcode, SDNode node> {
  def _8ri : Fox32Inst<(outs GPR8:$dst), (ins GPR8:$dst_in, i8imm:$src),
                       !strconcat(opcode, ".8 $dst, $src")> {
    let Pattern = [(set i8:$dst, (node i8:$dst_in, imm:$src))];
    let Constraints = "$dst_in = $dst";
  }

  def _16ri : Fox32Inst<(outs GPR16:$dst), (ins GPR16:$dst_in, i16imm:$src),
                        !strconcat(opcode, ".16 $dst, $src")> {
    let Pattern = [(set i16:$dst, (node i16:$dst_in, imm:$src))];
    let Constraints = "$dst_in = $dst";
  }

  def _32ri : Fox32Inst<(outs GPR32:$dst), (ins GPR32:$dst_in, i32imm:$src),
                        !strconcat(opcode, ".32 $dst, $src")> {
    let Pattern = [(set i32:$dst, (node i32:$dst_in, imm:$src))];
    let Constraints = "$dst_in = $dst";
  }
}

// Combined multiclass for both RR and RI variants
multiclass BinOp<string opcode, SDNode node> {
  defm "" : BinOpRR<opcode, node>;
  defm "" : BinOpRI<opcode, node>;
}

// Multiclass for MOV operations, register to register
multiclass MovRR<string opcode> {
  def _8rr : Fox32Inst<(outs GPR8:$dst), (ins GPR8:$src),
                       !strconcat(opcode, ".8 $dst, $src")>;

  def _16rr : Fox32Inst<(outs GPR16:$dst), (ins GPR16:$src),
                        !strconcat(opcode, ".16 $dst, $src")>;

  def _32rr : Fox32Inst<(outs GPR32:$dst), (ins GPR32:$src),
                        !strconcat(opcode, ".32 $dst, $src")>;
}

// Multiclass for MOV immediate
multiclass MovRI<string opcode> {
  def _8ri : Fox32Inst<(outs GPR8:$dst), (ins i8imm:$src),
                       !strconcat(opcode, ".8 $dst, $src")> {
    let Pattern = [(set i8:$dst, imm:$src)];
  }

  def _16ri : Fox32Inst<(outs GPR16:$dst), (ins i16imm:$src),
                        !strconcat(opcode, ".16 $dst, $src")> {
    let Pattern = [(set i16:$dst, imm:$src)];
  }

  def _32ri : Fox32Inst<(outs GPR32:$dst), (ins i32imm:$src),
                        !strconcat(opcode, ".32 $dst, $src")> {
    let Pattern = [(set i32:$dst, imm:$src)];
  }
}

multiclass Mov<string opcode> {
  defm "" : MovRR<opcode>;
  defm "" : MovRI<opcode>;
}

// Multiclass for bitwise NOT
multiclass NotOp<string opcode> {
  def _8r : Fox32Inst<(outs GPR8:$dst), (ins GPR8:$src),
                      !strconcat(opcode, ".8 $dst")> {
    let Pattern = [(set i8:$dst, (not i8:$src))];
    let Constraints = "$src = $dst";
  }

  def _16r : Fox32Inst<(outs GPR16:$dst), (ins GPR16:$src),
                       !strconcat(opcode, ".16 $dst")> {
    let Pattern = [(set i16:$dst, (not i16:$src))];
    let Constraints = "$src = $dst";
  }

  def _32r : Fox32Inst<(outs GPR32:$dst), (ins GPR32:$src),
                       !strconcat(opcode, ".32 $dst")> {
    let Pattern = [(set i32:$dst, (not i32:$src))];
    let Constraints = "$src = $dst";
  }
}

// Multiclass for increment
multiclass IncOp<string opcode> {
  def _8r : Fox32Inst<(outs GPR8:$dst), (ins GPR8:$src),
                      !strconcat(opcode, ".8 $dst")> {
    let Pattern = [(set i8:$dst, (add i8:$src, 1))];
    let Constraints = "$src = $dst";
  }

  def _16r : Fox32Inst<(outs GPR16:$dst), (ins GPR16:$src),
                       !strconcat(opcode, ".16 $dst")> {
    let Pattern = [(set i16:$dst, (add i16:$src, 1))];
    let Constraints = "$src = $dst";
  }

  def _32r : Fox32Inst<(outs GPR32:$dst), (ins GPR32:$src),
                       !strconcat(opcode, ".32 $dst")> {
    let Pattern = [(set i32:$dst, (add i32:$src, 1))];
    let Constraints = "$src = $dst";
  }
}

// Multiclass for decrement
multiclass DecOp<string opcode> {
  def _8r : Fox32Inst<(outs GPR8:$dst), (ins GPR8:$src),
                      !strconcat(opcode, ".8 $dst")> {
    let Pattern = [(set i8:$dst, (add i8:$src, -1))];
    let Constraints = "$src = $dst";
  }

  def _16r : Fox32Inst<(outs GPR16:$dst), (ins GPR16:$src),
                       !strconcat(opcode, ".16 $dst")> {
    let Pattern = [(set i16:$dst, (add i16:$src, -1))];
    let Constraints = "$src = $dst";
  }

  def _32r : Fox32Inst<(outs GPR32:$dst), (ins GPR32:$src),
                       !strconcat(opcode, ".32 $dst")> {
    let Pattern = [(set i32:$dst, (add i32:$src, -1))];
    let Constraints = "$src = $dst";
  }
}

// Multiclass for comparison operations (no output, just sets flags)
multiclass CmpRR<string opcode> {
  def _8rr : Fox32Inst<(outs), (ins GPR8:$lhs, GPR8:$rhs),
                       !strconcat(opcode, ".8 $lhs, $rhs")> {
    let Defs = [zero_flag, carry_flag];
  }

  def _16rr : Fox32Inst<(outs), (ins GPR16:$lhs, GPR16:$rhs),
                        !strconcat(opcode, ".16 $lhs, $rhs")> {
    let Defs = [zero_flag, carry_flag];
  }

  def _32rr : Fox32Inst<(outs), (ins GPR32:$lhs, GPR32:$rhs),
                        !strconcat(opcode, ".32 $lhs, $rhs")> {
    let Defs = [zero_flag, carry_flag];
  }
}

multiclass CmpRI<string opcode> {
  def _8ri : Fox32Inst<(outs), (ins GPR8:$lhs, i8imm:$rhs),
                       !strconcat(opcode, ".8 $lhs, $rhs")> {
    let Defs = [zero_flag, carry_flag];
  }

  def _16ri : Fox32Inst<(outs), (ins GPR16:$lhs, i16imm:$rhs),
                        !strconcat(opcode, ".16 $lhs, $rhs")> {
    let Defs = [zero_flag, carry_flag];
  }

  def _32ri : Fox32Inst<(outs), (ins GPR32:$lhs, i32imm:$rhs),
                        !strconcat(opcode, ".32 $lhs, $rhs")> {
    let Defs = [zero_flag, carry_flag];
  }
}

multiclass Cmp<string opcode> {
  defm "" : CmpRR<opcode>;
  defm "" : CmpRI<opcode>;
}

// Multiclass for load operations
multiclass Load<string opcode> {
  def _8rm : Fox32Inst<(outs GPR8:$dst), (ins i8imm:$addr),
                       !strconcat(opcode, ".8 $dst, [$addr]")> {
    let Pattern = [(set i8:$dst, (load imm:$addr))];
    let mayLoad = 1;
  }

  def _16rm : Fox32Inst<(outs GPR16:$dst), (ins i16imm:$addr),
                        !strconcat(opcode, ".16 $dst, [$addr]")> {
    let Pattern = [(set i16:$dst, (load imm:$addr))];
    let mayLoad = 1;
  }

  def _32rm : Fox32Inst<(outs GPR32:$dst), (ins i32imm:$addr),
                        !strconcat(opcode, ".32 $dst, [$addr]")> {
    let Pattern = [(set i32:$dst, (load imm:$addr))];
    let mayLoad = 1;
  }
}

// Multiclass for store operations
multiclass Store<string opcode> {
  def _8mr : Fox32Inst<(outs), (ins i32imm:$addr, GPR8:$src),
                       !strconcat(opcode, ".8 [$addr], $src")> {
    let Pattern = [(store i8:$src, imm:$addr)];
    let mayStore = 1;
  }

  def _16mr : Fox32Inst<(outs), (ins i32imm:$addr, GPR16:$src),
                        !strconcat(opcode, ".16 [$addr], $src")> {
    let Pattern = [(store i16:$src, imm:$addr)];
    let mayStore = 1;
  }

  def _32mr : Fox32Inst<(outs), (ins i32imm:$addr, GPR32:$src),
                        !strconcat(opcode, ".32 [$addr], $src")> {
    let Pattern = [(store i32:$src, imm:$addr)];
    let mayStore = 1;
  }
}

// Multiclass for load operations with register+offset addressing
multiclass LoadRegImm<string opcode> {
  def _8rri : Fox32Inst<(outs GPR8:$dst), (ins MEMri:$addr),
                        !strconcat(opcode, ".8 $dst, [$addr]")> {
    let mayLoad = 1;
  }

  def _16rri : Fox32Inst<(outs GPR16:$dst), (ins MEMri:$addr),
                         !strconcat(opcode, ".16 $dst, [$addr]")> {
    let mayLoad = 1;
  }

  def _32rri : Fox32Inst<(outs GPR32:$dst), (ins MEMri:$addr),
                         !strconcat(opcode, ".32 $dst, [$addr]")> {
    let mayLoad = 1;
  }
}

// Multiclass for store operations with register+offset addressing
multiclass StoreRegImm<string opcode> {
  def _8rri : Fox32Inst<(outs), (ins MEMri:$addr, GPR8:$src),
                        !strconcat(opcode, ".8 [$addr], $src")> {
    let mayStore = 1;
  }

  def _16rri : Fox32Inst<(outs), (ins MEMri:$addr, GPR16:$src),
                         !strconcat(opcode, ".16 [$addr], $src")> {
    let mayStore = 1;
  }

  def _32rri : Fox32Inst<(outs), (ins MEMri:$addr, GPR32:$src),
                         !strconcat(opcode, ".32 [$addr], $src")> {
    let mayStore = 1;
  }
}

// Multiclass for load operations with register+register addressing
multiclass LoadRegReg<string opcode> {
  def _8rrr : Fox32Inst<(outs GPR8:$dst), (ins MEMrr:$addr),
                        !strconcat(opcode, ".8 $dst, [$addr]")> {
    let mayLoad = 1;
  }

  def _16rrr : Fox32Inst<(outs GPR16:$dst), (ins MEMrr:$addr),
                         !strconcat(opcode, ".16 $dst, [$addr]")> {
    let mayLoad = 1;
  }

  def _32rrr : Fox32Inst<(outs GPR32:$dst), (ins MEMrr:$addr),
                         !strconcat(opcode, ".32 $dst, [$addr]")> {
    let mayLoad = 1;
  }
}

// Multiclass for store operations with register+register addressing
multiclass StoreRegReg<string opcode> {
  def _8rrr : Fox32Inst<(outs), (ins MEMrr:$addr, GPR8:$src),
                        !strconcat(opcode, ".8 [$addr], $src")> {
    let mayStore = 1;
  }

  def _16rrr : Fox32Inst<(outs), (ins MEMrr:$addr, GPR16:$src),
                         !strconcat(opcode, ".16 [$addr], $src")> {
    let mayStore = 1;
  }

  def _32rrr : Fox32Inst<(outs), (ins MEMrr:$addr, GPR32:$src),
                         !strconcat(opcode, ".32 [$addr], $src")> {
    let mayStore = 1;
  }
}

// Multiclass for stack operations (PUSH)
multiclass Push<string opcode> {
  def _8r : Fox32Inst<(outs), (ins GPR8:$src), !strconcat(opcode, ".8 $src")> {
    let Defs = [rsp];
    let Uses = [rsp];
    let mayStore = 1;
  }

  def _16r
      : Fox32Inst<(outs), (ins GPR16:$src), !strconcat(opcode, ".16 $src")> {
    let Defs = [rsp];
    let Uses = [rsp];
    let mayStore = 1;
  }

  def _32r
      : Fox32Inst<(outs), (ins GPR32:$src), !strconcat(opcode, ".32 $src")> {
    let Defs = [rsp];
    let Uses = [rsp];
    let mayStore = 1;
  }
}

// Multiclass for stack operations (POP)
multiclass Pop<string opcode> {
  def _8r : Fox32Inst<(outs GPR8:$dst), (ins), !strconcat(opcode, ".8 $dst")> {
    let Defs = [rsp];
    let Uses = [rsp];
    let mayLoad = 1;
  }

  def _16r
      : Fox32Inst<(outs GPR16:$dst), (ins), !strconcat(opcode, ".16 $dst")> {
    let Defs = [rsp];
    let Uses = [rsp];
    let mayLoad = 1;
  }

  def _32r
      : Fox32Inst<(outs GPR32:$dst), (ins), !strconcat(opcode, ".32 $dst")> {
    let Defs = [rsp];
    let Uses = [rsp];
    let mayLoad = 1;
  }
}

multiclass ShiftOpRR<string opcode, SDNode node> {
  def _8rr : Fox32Inst<(outs GPR8:$dst), (ins GPR8:$dst_in, GPR8:$src),
                       !strconcat(opcode, ".8 $dst, $src")> {
    let Pattern = [(set i8:$dst, (node i8:$dst_in, i8:$src))];
    let Constraints = "$dst_in = $dst";
  }

  def _16rr : Fox32Inst<(outs GPR16:$dst), (ins GPR16:$dst_in, GPR16:$src),
                        !strconcat(opcode, ".16 $dst, $src")> {
    let Pattern = [(set i16:$dst, (node i16:$dst_in, i16:$src))];
    let Constraints = "$dst_in = $dst";
  }

  def _32rr : Fox32Inst<(outs GPR32:$dst), (ins GPR32:$dst_in, GPR32:$src),
                        !strconcat(opcode, " $dst, $src")> {
    let Pattern = [(set i32:$dst, (node i32:$dst_in, i32:$src))];
    let Constraints = "$dst_in = $dst";
  }
}

multiclass ShiftOpRI<string opcode> {
  def _8ri : Fox32Inst<(outs GPR8:$dst), (ins GPR8:$dst_in, i8imm:$src),
                       !strconcat(opcode, ".8 $dst, $src")> {
    let Constraints = "$dst_in = $dst";
  }

  def _16ri : Fox32Inst<(outs GPR16:$dst), (ins GPR16:$dst_in, i8imm:$src),
                        !strconcat(opcode, ".16 $dst, $src")> {
    let Constraints = "$dst_in = $dst";
  }

  def _32ri : Fox32Inst<(outs GPR32:$dst), (ins GPR32:$dst_in, i8imm:$src),
                        !strconcat(opcode, " $dst, $src")> {
    let Constraints = "$dst_in = $dst";
  }
}

multiclass ShiftOp<string opcode, SDNode node> {
  defm "" : ShiftOpRR<opcode, node>;
  defm "" : ShiftOpRI<opcode>;
}
