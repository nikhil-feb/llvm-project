add_llvm_component_group(ToyISA)

set(LLVM_TARGET_DEFINITIONS ToyISA.td)


tablegen(LLVM ToyISAGenCallingConv.inc -gen-callingconv)


add_public_tablegen_target(ToyISACommonTableGen)

add_llvm_target(ToyISACodeGen
  ToyISATargetMachine.cpp

  LINK_COMPONENTS
  AsmPrinter
  CodeGen
  CodeGenTypes
  Core
  MC
  SelectionDAG
  Support
  Target
  TargetParser

  ADD_TO_COMPONENT
  ToyISA
  )

