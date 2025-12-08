// This is the top tablegen file that pulls in other code
// specific to the Fox32 target.

include "llvm/Target/Target.td"

include "Fox32RegisterInfo.td"
include "Fox32InstrInfo.td"

def : ProcessorModel<"generic", NoSchedModel, []>;

def Fox32InstrInfo : InstrInfo {}

def Fox32 : Target {
    let InstructionSet = Fox32InstrInfo;
}
