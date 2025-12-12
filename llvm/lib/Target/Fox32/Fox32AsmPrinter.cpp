#include "Fox32.h"
#include "Fox32Subtarget.h"
#include "Fox32TargetMachine.h"
#include "MCTargetDesc/Fox32MCInstPrinter.h"
#include "TargetInfo/Fox32TargetInfo.h"

#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/TargetRegistry.h"

#define DEBUG_TYPE "fox32-asm-printer"

using namespace llvm;

namespace {
class Fox32AsmPrinter : public AsmPrinter {
public:
  Fox32AsmPrinter(TargetMachine &TM, std::unique_ptr<MCStreamer> Streamer)
      : AsmPrinter(TM, std::move(Streamer)) {}

  StringRef getPassName() const override { return "Fox32 Assembly Printer"; }

  void emitInstruction(const MachineInstr *MI) override;

  // Lower the MachineInstr to MCInst
  void lowerInstruction(const MachineInstr &MI, MCInst &Inst);

  // bool lowerPseudoInstExpansion(const MachineInstr *MI, MCInst &Inst);
private:
  MCOperand lowerSymbolOperand(const MachineOperand &MO, MCSymbol *Sym);
};
} // end anonymous namespace

MCOperand Fox32AsmPrinter::lowerSymbolOperand(const MachineOperand &MO,
                                              MCSymbol *Sym) {
  auto &Ctx = OutContext;
  const MCExpr *Expr = MCSymbolRefExpr::create(Sym, Ctx);

  if (!MO.isJTI() && !MO.isMBB() && MO.getOffset()) {
    Expr = MCBinaryExpr::createAdd(
        Expr, MCConstantExpr::create(MO.getOffset(), Ctx), Ctx);
  }

  return MCOperand::createExpr(Expr);
}

void Fox32AsmPrinter::lowerInstruction(const MachineInstr &MI, MCInst &Inst) {
  Inst.setOpcode(MI.getOpcode());

  for (unsigned i = 0, e = MI.getNumOperands(); i != e; ++i) {
    const MachineOperand &MO = MI.getOperand(i);

    MCOperand MCOp;
    switch (MO.getType()) {
    case MachineOperand::MO_Register: {
      if (MO.isUse() && MI.isRegTiedToDefOperand(i)) {
        MCOp = MCOperand::createReg(MI.getOperand(i).getReg());
      } else {
        MCOp = MCOperand::createReg(MO.getReg());
      }
    } break;
    case MachineOperand::MO_Immediate: {
      MCOp = MCOperand::createImm(MO.getImm());
    } break;
    case MachineOperand::MO_MachineBasicBlock: {
      MCOp = lowerSymbolOperand(MO, MO.getMBB()->getSymbol());
    } break;
    case MachineOperand::MO_GlobalAddress: {
      MCOp = lowerSymbolOperand(MO, getSymbol(MO.getGlobal()));
    } break;
    case MachineOperand::MO_ExternalSymbol: {
      MCOp =
          lowerSymbolOperand(MO, GetExternalSymbolSymbol(MO.getSymbolName()));
    } break;
    case MachineOperand::MO_RegisterMask: {
      // Register masks are used for call clobbers, skip them
    } break;
    default:
      llvm_unreachable("Unsupported operand type");
    }
    Inst.addOperand(MCOp);
  }
}

void Fox32AsmPrinter::emitInstruction(const MachineInstr *MI) {
  // Lower the instruction to MCInst
  MCInst Inst;
  lowerInstruction(*MI, Inst);
  EmitToStreamer(*OutStreamer, Inst);
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeFox32AsmPrinter() {
  RegisterAsmPrinter<Fox32AsmPrinter> X(getTheFox32Target());
}
