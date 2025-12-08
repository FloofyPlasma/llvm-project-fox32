// This is the top tablegen file that pulls in other code
// specific to the Fox32 target.

include "llvm/Target/Target.td"

include "Fox32RegisterInfo.td"

def : ProcessorModel<"generic", NoSchedModel, []>;

def Fox32 : Target {
}
