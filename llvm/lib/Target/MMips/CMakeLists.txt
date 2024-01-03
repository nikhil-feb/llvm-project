set(MMipsCodeGen_sources
    MMipsTargetInfo.cpp
    MMipsTargetMachine.cpp
)
add_llvm_library(LLVMMMipsInfo
    MMipsCodeGen_sources
)

add_subdirectory(MCTargetDesc)