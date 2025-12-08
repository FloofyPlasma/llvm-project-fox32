add_llvm_component_group(Fox32)

set(LLVM_TARGET_DEFINITIONS Fox32.td)

tablegen(LLVM Fox32GenRegisterInfo.inc -gen-register-info)

add_public_tablegen_target(Fox32CommonTableGen)

add_llvm_target(Fox32CodeGen
    Fox32TargetMachine.cpp
    Fox32RegisterInfo.cpp
    MCTargetDesc/Fox32MCTargetDesc.cpp

    LINK_COMPONENTS
    MC
    TargetParser
    Target

    ADD_TO_COMPONENT
    Fox32
)
