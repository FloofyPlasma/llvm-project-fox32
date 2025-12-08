// Fox32 CPU Registers

class Fox32Reg<string n> : Register<n> {
    let Namespace = "Fox32";
}

class Fox32GPRReg<string n> : Register<n> {
    let Namespace = "Fox32";
}

let Namespace = "Fox32" in {
    def r0  : Fox32GPRReg<"r0">;
    def r1  : Fox32GPRReg<"r1">;
    def r2  : Fox32GPRReg<"r2">;
    def r3  : Fox32GPRReg<"r3">;
    def r4  : Fox32GPRReg<"r4">;
    def r5  : Fox32GPRReg<"r5">;
    def r6  : Fox32GPRReg<"r6">;
    def r7  : Fox32GPRReg<"r7">;
    def r8  : Fox32GPRReg<"r8">;
    def r9  : Fox32GPRReg<"r9">;
    def r10 : Fox32GPRReg<"r10">;
    def r11 : Fox32GPRReg<"r11">;
    def r12 : Fox32GPRReg<"r12">;
    def r13 : Fox32GPRReg<"r13">;
    def r14 : Fox32GPRReg<"r14">;
    def r15 : Fox32GPRReg<"r15">;
    def r16 : Fox32GPRReg<"r16">;
    def r17 : Fox32GPRReg<"r17">;
    def r18 : Fox32GPRReg<"r18">;
    def r19 : Fox32GPRReg<"r19">;
    def r20 : Fox32GPRReg<"r20">;
    def r21 : Fox32GPRReg<"r21">;
    def r22 : Fox32GPRReg<"r22">;
    def r23 : Fox32GPRReg<"r23">;
    def r24 : Fox32GPRReg<"r24">;
    def r25 : Fox32GPRReg<"r25">;
    def r26 : Fox32GPRReg<"r26">;
    def r27 : Fox32GPRReg<"r27">;
    def r28 : Fox32GPRReg<"r28">;
    def r29 : Fox32GPRReg<"r29">;
    def r30 : Fox32GPRReg<"r30">;
    def r31 : Fox32GPRReg<"r31">;

    //== Special registers ==//
    //=======================//
    def rsp  : Fox32Reg<"rsp">;  // current stack pointer
    def resp : Fox32Reg<"resp">; // exception stack pointer
    def rfp  : Fox32Reg<"rfp">;  // frame pointer
    def rip  : Fox32Reg<"rip">;  // instruction pointer

    // Condition flags
    def zero_flag  : Fox32Reg<"zero_flag">;
    def carry_flag : Fox32Reg<"carry_flag">;

    // Other flags
    def swap_sp_flag      : Fox32Reg<"swap_sp_flag">;
    def interrupt_flag    : Fox32Reg<"interrupt_flag">;

    // MMU state
    def mmu_enabled_flag  : Fox32Reg<"mmu_enabled_flag">;
    def page_directory_ptr : Fox32Reg<"page_directory_ptr">;
}

//=== -----------------
// Register Classes
//=== -----------------

def GPR32 : RegisterClass<"Fox32",
    [i32],      // supported type
    32,         // number of registers
    (add
        r0, r1, r2, r3, r4, r5, r6, r7,
        r8, r9, r10, r11, r12, r13, r14, r15,
        r16, r17, r18, r19, r20, r21, r22, r23,
        r24, r25, r26, r27, r28, r29, r30, r31
    )
>;

def SPReg32 : RegisterClass<"Fox32",
    [i32],
    3,
    (add
        rsp, resp, rfp
    )
>;

def IPReg32 : RegisterClass<"Fox32",
    [i32],
    1,
    (add
        rip
    )
>;

def FlagReg32 : RegisterClass<"Fox32",
    [i32],
    4,
    (add
        zero_flag, carry_flag, interrupt_flag, swap_sp_flag
    )
>;

def MMUReg32 : RegisterClass<"Fox32",
    [i32],
    2,
    (add
        mmu_enabled_flag, page_directory_ptr
    )
>;

let isAllocatable = false in {
    def CPUStackReg  : RegisterClass<"Fox32", [i32], 3, (add rsp, resp, rfp)>;
    def CPUIPReg     : RegisterClass<"Fox32", [i32], 1, (add rip)>;
    def CPUFlagReg   : RegisterClass<"Fox32", [i32], 4, (add zero_flag, carry_flag, interrupt_flag, swap_sp_flag)>;
    def CPUMMUReg    : RegisterClass<"Fox32", [i32], 2, (add mmu_enabled_flag, page_directory_ptr)>;
}
