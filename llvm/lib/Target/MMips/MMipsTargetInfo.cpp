#include "MMips.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;
Target llvm::TheMMipsTarget;
extern "C" void LLVMInitializeMMipsTargetInfo() {
RegisterTarget<Triple::toy> X(TheMMipsTarget, "mmips",
"MMips");
}