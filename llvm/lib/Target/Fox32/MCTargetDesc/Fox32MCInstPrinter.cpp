#include "Fox32MCInstPrinter.h"
#include "Fox32InstrInfo.h"

#define DEBUG_TYPE "fox32-mcinst-printer"

using namespace llvm;

#define PRINT_ALIAS_INSTR
#include "Fox32GenAsmWriter.inc"

void Fox32InstPrinter::printInst(const MCInst *MI, uint64_t Address,
                                 StringRef Annot, const MCSubtargetInfo &STI,
                                 raw_ostream &O) {
  // check if we have an alias
  if (!printAliasInstr(MI, Address, O)) {
    printInstruction(MI, Address, O);
  }
  printAnnotation(O, Annot);
}

void Fox32InstPrinter::printRegName(raw_ostream &IS, MCRegister Reg) {
  IS << StringRef(getRegisterName(Reg)).lower();
}

void Fox32InstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                    raw_ostream &O) {
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isReg()) {
    printRegName(O, Op.getReg());
    return;
  }

  if (Op.isImm()) {
    O << Op.getImm();
    return;
  }

  assert(Op.isExpr() && "unknown operand type");
  const MCExpr *Expr = Op.getExpr();
  if (const MCSymbolRefExpr *SRE = dyn_cast<MCSymbolRefExpr>(Expr)) {
    O << SRE->getSymbol();
  } else {
    O << "(" << Expr << ")";
  }
}
