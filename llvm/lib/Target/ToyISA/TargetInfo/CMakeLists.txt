add_llvm_component_library(LLVMToyISAInfo
  ToyISATargetInfo.cpp

  LINK_COMPONENTS
  MC
  Support

  ADD_TO_COMPONENT
  ToyISA
  )
