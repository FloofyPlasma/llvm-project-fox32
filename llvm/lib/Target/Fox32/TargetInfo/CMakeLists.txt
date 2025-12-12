add_llvm_component_library(LLVMFox32Info
    Fox32TargetInfo.cpp

    LINK_COMPONENTS
    MC
    Support

    ADD_TO_COMPONENT
    Fox32
)
