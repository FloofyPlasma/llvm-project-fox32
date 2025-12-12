// Fox32 CPU Registers

class Fox32Reg<string n> : Register<n> { let Namespace = "Fox32"; }

class Fox32GPRReg<string n, bits<5> num> : Register<n> {
  let Namespace = "Fox32";
  let HWEncoding{4 -0} = num;
}

class Fox32SpecialReg<string n, bits<6> num> : Register<n> {
  let Namespace = "Fox32";
  let HWEncoding{5 -0} = num;
}

let Namespace = "Fox32" in {
  // Generate r0-r31 using foreach loop
  foreach i = 0 -31 in { def r#i : Fox32GPRReg<"r"#i, i>; }

  //== Special registers ==//
  //=======================//
  def rsp : Fox32SpecialReg<"rsp", 32>;   // current stack pointer
  def resp : Fox32SpecialReg<"resp", 33>; // exception stack pointer
  def rfp : Fox32SpecialReg<"rfp", 34>;   // frame pointer
  def rip : Fox32Reg<"rip">; // instruction pointer (not directly encoded)

  // Condition flags
  def zero_flag : Fox32Reg<"zero_flag">;
  def carry_flag : Fox32Reg<"carry_flag">;

  // Other flags
  def swap_sp_flag : Fox32Reg<"swap_sp_flag">;
  def interrupt_flag : Fox32Reg<"interrupt_flag">;

  // MMU state
  def mmu_enabled_flag : Fox32Reg<"mmu_enabled_flag">;
  def page_directory_ptr : Fox32Reg<"page_directory_ptr">;
}

// Primary 32-bit general purpose register class
def GPR32 : RegisterClass<"Fox32", [i32], 32, (sequence "r%u", 0, 31)>;

// 16-bit register class (alaises the same physical registers as GPR32)
def GPR16 : RegisterClass<"Fox32", [i16], 16, (sequence "r%u", 0, 31)> {
  let CopyCost =
      -1; // Copy between GPR16 and GPR32 is free (same physical registers)
}

// 8-bit register class (alaises the same physical registers as GPR32)
def GPR8 : RegisterClass<"Fox32", [i8], 8, (sequence "r%u", 0, 31)> {
  let CopyCost =
      -1; // Copy between GPR8 and GPR32 is free (same physical registers)
}

// Special-purpose register classes (allocatable for specific pruposes)
def SPReg32 : RegisterClass<"Fox32", [i32], 32, (add rsp, resp, rfp)>;

def IPReg32 : RegisterClass<"Fox32", [i32], 32, (add rip)>;

def FlagReg32
    : RegisterClass<"Fox32", [i32], 32,
                    (add zero_flag, carry_flag, interrupt_flag, swap_sp_flag)>;

def MMUReg32 : RegisterClass<"Fox32", [i32], 32,
                             (add mmu_enabled_flag, page_directory_ptr)>;

// Non-allocatable register classes (reserved for specific uses)
let isAllocatable = 0 in {
  def CPUStackReg : RegisterClass<"Fox32", [i32], 32, (add rsp, resp, rfp)>;

  def CPUIPReg : RegisterClass<"Fox32", [i32], 32, (add rip)>;

  def CPUFlagReg : RegisterClass<"Fox32", [i32], 32,
                                 (add zero_flag, carry_flag, interrupt_flag,
                                     swap_sp_flag)>;

  def CPUMMUReg : RegisterClass<"Fox32", [i32], 32,
                                (add mmu_enabled_flag, page_directory_ptr)>;
}
