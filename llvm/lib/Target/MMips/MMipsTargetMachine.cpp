#include "MMipsTargetMachine.h"
#include "MMips.h"
#include "MMipsFrameLowering.h"
#include "MMipsInstrInfo.h"
#include "MMipsISelLowering.h"
#include "MMipsSelectionDAGInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/IR/Module.h"
#include "llvm/PassManager.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;
MMipsTargetMachine::MMipsTargetMachine(const Target &T, StringRef TT,
StringRef CPU, StringRef FS, const TargetOptions &Options,
Reloc::Model RM, CodeModel::Model CM,
CodeGenOpt::Level OL)
: LLVMTargetMachine(T, TT, CPU, FS, Options, RM, CM,
OL),
Subtarget(TT, CPU, FS, *this) {
initAsmInfo();
}
namespace {
class MMipsPassConfig : public TargetPassConfig {
public:
MMipsPassConfig(MMipsTargetMachine *TM, PassManagerBase &PM)
: TargetPassConfig(TM, PM) {}
MMipsTargetMachine &getMMipsTargetMachine() const {
return getTM<MMipsTargetMachine>();
}
virtual bool addPreISel();
virtual bool addInstSelector();
virtual bool addPreEmitPass();
};
} // namespace
TargetPassConfig
*MMipsTargetMachine::createPassConfig(PassManagerBase &PM) {
return new MMipsPassConfig(this, PM);
}
bool MMipsPassConfig::addPreISel() { return false; }
bool MMipsPassConfig::addInstSelector() {
addPass(createMMipsISelDag(getMMipsTargetMachine(),
getOptLevel()));
return false;
}
bool MMipsPassConfig::addPreEmitPass() { return false; }
// Force static initialization.
extern "C" void LLVMInitializeMMipsTarget() {
RegisterTargetMachine<MMipsTargetMachine> X(TheMMipsTarget);
}
void MMipsTargetMachine::addAnalysisPasses(PassManagerBase
&PM) {}