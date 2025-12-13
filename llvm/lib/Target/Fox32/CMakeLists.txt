add_llvm_component_group(Fox32)

set(LLVM_TARGET_DEFINITIONS Fox32.td)

tablegen(LLVM Fox32GenRegisterInfo.inc -gen-register-info)
tablegen(LLVM Fox32GenSubtargetInfo.inc -gen-subtarget)
tablegen(LLVM Fox32GenInstrInfo.inc -gen-instr-info)
tablegen(LLVM Fox32GenCallingConv.inc -gen-callingconv)
tablegen(LLVM Fox32GenDAGISel.inc -gen-dag-isel)
tablegen(LLVM Fox32GenAsmWriter.inc -gen-asm-writer)

add_public_tablegen_target(Fox32CommonTableGen)

add_llvm_target(Fox32CodeGen
    Fox32AsmPrinter.cpp
    Fox32FrameLowering.cpp
    Fox32InstrInfo.cpp
    Fox32ISelDAGToDAG.cpp
    Fox32ISelLowering.cpp
    Fox32RegisterInfo.cpp
    Fox32Subtarget.cpp
    Fox32TargetMachine.cpp
    Fox32TargetObjectFile.cpp

    LINK_COMPONENTS
    AsmPrinter
    CodeGen
    Core
    SelectionDAG
    MC
    TargetParser
    Target

    ADD_TO_COMPONENT
    Fox32
)

add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
