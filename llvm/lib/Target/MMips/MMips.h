#ifndef TARGET_MMips_H
#define TARGET_MMips_H
#include "MCTargetDesc/MMipsMCTargetDesc.h"
#include "llvm/Target/TargetMachine.h"
namespace llvm {
class TargetMachine;
class MMipsTargetMachine;
FunctionPass *createMMipsISelDag(MMipsTargetMachine &TM,
CodeGenOpt::Level OptLevel);
} // end namespace llvm;
#endif