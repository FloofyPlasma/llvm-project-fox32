//===-- Fox32CallingConv.td - Calling Conventions Fox32 ----*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file describes the calling conventions for the Fox32 architecture.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Fox32 Argument Calling Convention
//===----------------------------------------------------------------------===//

// The Fox32 calling convention passes arguments in registers r0-r7,
// with overflow going to the stack. Return values use r0.

def CC_Fox32 : CallingConv<[
  // Promote i8/i16 to i32 if needed (or keep them separate)
  // For now, let's keep type-specific assignments
  
  // i8 arguments passed in GPR8 registers
  CCIfType<[i8], CCAssignToReg<[r0, r1, r2, r3, r4, r5, r6, r7]>>,
  
  // i16 arguments passed in GPR16 registers
  CCIfType<[i16], CCAssignToReg<[r0, r1, r2, r3, r4, r5, r6, r7]>>,
  
  // i32 arguments passed in GPR32 registers
  CCIfType<[i32], CCAssignToReg<[r0, r1, r2, r3, r4, r5, r6, r7]>>,
  
  // Stack slots for overflow arguments (aligned appropriately)
  CCIfType<[i8],  CCAssignToStack<1, 1>>,
  CCIfType<[i16], CCAssignToStack<2, 2>>,
  CCIfType<[i32], CCAssignToStack<4, 4>>
]>;

//===----------------------------------------------------------------------===//
// Fox32 Return Value Calling Convention
//===----------------------------------------------------------------------===//

def RetCC_Fox32 : CallingConv<[
  // Return i8 values in r0 (as GPR8)
  CCIfType<[i8], CCAssignToReg<[r0]>>,
  
  // Return i16 values in r0 (as GPR16)
  CCIfType<[i16], CCAssignToReg<[r0]>>,
  
  // Return i32 values in r0 (as GPR32)
  CCIfType<[i32], CCAssignToReg<[r0]>>
]>;

//===----------------------------------------------------------------------===//
// Callee-saved Registers
//===----------------------------------------------------------------------===//

def CSR_Fox32 : CalleeSavedRegs<(add r8, r9, r10, r11, r12, r13, r14, r15,
                                      r16, r17, r18, r19, r20, r21, r22, r23,
                                      rfp)>;
