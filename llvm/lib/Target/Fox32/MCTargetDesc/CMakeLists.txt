add_llvm_component_library(LLVMFox32Desc
    Fox32MCAsmInfo.cpp
    Fox32MCInstPrinter.cpp
    Fox32MCTargetDesc.cpp

    LINK_COMPONENTS
    MC
    Fox32Info
    Support
    TargetParser

    ADD_TO_COMPONENT
    Fox32
)
