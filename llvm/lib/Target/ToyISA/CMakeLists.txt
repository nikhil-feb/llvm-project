add_llvm_component_group(ToyISA)

set(LLVM_TARGET_DEFINITIONS ToyISA.td)

tablegen(LLVM ToyISAGenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM ToyISAGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM ToyISAGenCallingConv.inc -gen-callingconv)
tablegen(LLVM ToyISAGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM ToyISAGenDisassemblerTables.inc -gen-disassembler)
tablegen(LLVM ToyISAGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM ToyISAGenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM ToyISAGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM ToyISAGenSubtargetInfo.inc -gen-subtarget)


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
  add_subdirectory(TargetInfo)
