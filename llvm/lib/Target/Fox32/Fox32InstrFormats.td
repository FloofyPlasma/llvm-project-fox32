//===-- Fox32InstrFormats.td - Fox32 Instruction Formats --------------------===//
// This file contains the instruction formats for the Fox32 architecture.
//===----------------------------------------------------------------------===//

class Fox32Inst<dag outs, dag ins, string asmString> : Instruction {
    let Namespace = "Fox32";
    let OutOperandList = outs;
    let InOperandList = ins;
    let AsmString = asmString;
}

class PseudoFox32Inst<dag outs, dag ins, string asmString> : Fox32Inst<outs, ins, asmString> {
    let isPseudo = 1;
    let isCodeGenOnly = 1;
}
