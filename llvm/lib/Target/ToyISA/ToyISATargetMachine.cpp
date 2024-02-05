//===-- ToyISATargetMachine.cpp - Define TargetMachine for ToyISA -----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

#include "ToyISATargetMachine.h"
#include "LeonPasses.h"
#include "ToyISA.h"
#include "ToyISAMachineFunctionInfo.h"
#include "ToyISATargetObjectFile.h"
#include "TargetInfo/ToyISATargetInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/MC/TargetRegistry.h"
#include <optional>
using namespace llvm;

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeToyISATarget() {
  // Register the target.
  RegisterTargetMachine<ToyISAV8TargetMachine> X(getTheToyISATarget());
  RegisterTargetMachine<ToyISAV9TargetMachine> Y(getTheToyISAV9Target());
  RegisterTargetMachine<ToyISAelTargetMachine> Z(getTheToyISAelTarget());

  PassRegistry &PR = *PassRegistry::getPassRegistry();
  initializeToyISADAGToDAGISelPass(PR);
}

static cl::opt<bool>
    BranchRelaxation("ToyISA-enable-branch-relax", cl::Hidden, cl::init(true),
                     cl::desc("Relax out of range conditional branches"));

static std::string computeDataLayout(const Triple &T, bool is64Bit) {
  // ToyISA is typically big endian, but some are little.
  std::string Ret = T.getArch() == Triple::toyisa ? "e" : "E";
  Ret += "-m:e";

  // Some ABIs have 32bit pointers.
  if (!is64Bit)
    Ret += "-p:32:32";

  // Alignments for 64 bit integers.
  Ret += "-i64:64";

  // On ToyISAV9 128 floats are aligned to 128 bits, on others only to 64.
  // On ToyISAV9 registers can hold 64 or 32 bits, on others only 32.
  if (is64Bit)
    Ret += "-n32:64";
  else
    Ret += "-f128:64-n32";

  if (is64Bit)
    Ret += "-S128";
  else
    Ret += "-S64";

  return Ret;
}

static Reloc::Model getEffectiveRelocModel(std::optional<Reloc::Model> RM) {
  return RM.value_or(Reloc::Static);
}

// Code models. Some only make sense for 64-bit code.
//
// SunCC  Reloc   CodeModel  Constraints
// abs32  Static  Small      text+data+bss linked below 2^32 bytes
// abs44  Static  Medium     text+data+bss linked below 2^44 bytes
// abs64  Static  Large      text smaller than 2^31 bytes
// pic13  PIC_    Small      GOT < 2^13 bytes
// pic32  PIC_    Medium     GOT < 2^32 bytes
//
// All code models require that the text segment is smaller than 2GB.
static CodeModel::Model
getEffectiveToyISACodeModel(std::optional<CodeModel::Model> CM, Reloc::Model RM,
                           bool Is64Bit, bool JIT) {
  if (CM) {
    if (*CM == CodeModel::Tiny)
      report_fatal_error("Target does not support the tiny CodeModel", false);
    if (*CM == CodeModel::Kernel)
      report_fatal_error("Target does not support the kernel CodeModel", false);
    return *CM;
  }
  if (Is64Bit) {
    if (JIT)
      return CodeModel::Large;
    return RM == Reloc::PIC_ ? CodeModel::Small : CodeModel::Medium;
  }
  return CodeModel::Small;
}

/// Create an ILP32 architecture model
ToyISATargetMachine::ToyISATargetMachine(const Target &T, const Triple &TT,
                                       StringRef CPU, StringRef FS,
                                       const TargetOptions &Options,
                                       std::optional<Reloc::Model> RM,
                                       std::optional<CodeModel::Model> CM,
                                       CodeGenOpt::Level OL, bool JIT,
                                       bool is64bit)
    : LLVMTargetMachine(T, computeDataLayout(TT, is64bit), TT, CPU, FS, Options,
                        getEffectiveRelocModel(RM),
                        getEffectiveToyISACodeModel(
                            CM, getEffectiveRelocModel(RM), is64bit, JIT),
                        OL),
      TLOF(std::make_unique<ToyISAELFTargetObjectFile>()),
      Subtarget(TT, std::string(CPU), std::string(FS), *this, is64bit),
      is64Bit(is64bit) {
  initAsmInfo();
}

ToyISATargetMachine::~ToyISATargetMachine() = default;

const ToyISASubtarget *
ToyISATargetMachine::getSubtargetImpl(const Function &F) const {
  Attribute CPUAttr = F.getFnAttribute("target-cpu");
  Attribute FSAttr = F.getFnAttribute("target-features");

  std::string CPU =
      CPUAttr.isValid() ? CPUAttr.getValueAsString().str() : TargetCPU;
  std::string FS =
      FSAttr.isValid() ? FSAttr.getValueAsString().str() : TargetFS;

  // FIXME: This is related to the code below to reset the target options,
  // we need to know whether or not the soft float flag is set on the
  // function, so we can enable it as a subtarget feature.
  bool softFloat = F.getFnAttribute("use-soft-float").getValueAsBool();

  if (softFloat)
    FS += FS.empty() ? "+soft-float" : ",+soft-float";

  auto &I = SubtargetMap[CPU + FS];
  if (!I) {
    // This needs to be done before we create a new subtarget since any
    // creation will depend on the TM and the code generation flags on the
    // function that reside in TargetOptions.
    resetTargetOptions(F);
    I = std::make_unique<ToyISASubtarget>(TargetTriple, CPU, FS, *this,
                                          this->is64Bit);
  }
  return I.get();
}

MachineFunctionInfo *ToyISATargetMachine::createMachineFunctionInfo(
    BumpPtrAllocator &Allocator, const Function &F,
    const TargetSubtargetInfo *STI) const {
  return ToyISAMachineFunctionInfo::create<ToyISAMachineFunctionInfo>(Allocator,
                                                                    F, STI);
}

namespace {
/// ToyISA Code Generator Pass Configuration Options.
class ToyISAPassConfig : public TargetPassConfig {
public:
  ToyISAPassConfig(ToyISATargetMachine &TM, PassManagerBase &PM)
    : TargetPassConfig(TM, PM) {}

  ToyISATargetMachine &getToyISATargetMachine() const {
    return getTM<ToyISATargetMachine>();
  }

  void addIRPasses() override;
  bool addInstSelector() override;
  void addPreEmitPass() override;
};
} // namespace

TargetPassConfig *ToyISATargetMachine::createPassConfig(PassManagerBase &PM) {
  return new ToyISAPassConfig(*this, PM);
}

void ToyISAPassConfig::addIRPasses() {
  addPass(createAtomicExpandPass());

  TargetPassConfig::addIRPasses();
}

bool ToyISAPassConfig::addInstSelector() {
  addPass(createToyISAISelDag(getToyISATargetMachine()));
  return false;
}

void ToyISAPassConfig::addPreEmitPass(){
  if (BranchRelaxation)
    addPass(&BranchRelaxationPassID);

  addPass(createToyISADelaySlotFillerPass());

  if (this->getToyISATargetMachine().getSubtargetImpl()->insertNOPLoad())
  {
    addPass(new InsertNOPLoad());
  }
  if (this->getToyISATargetMachine().getSubtargetImpl()->detectRoundChange()) {
    addPass(new DetectRoundChange());
  }
  if (this->getToyISATargetMachine().getSubtargetImpl()->fixAllFDIVSQRT())
  {
    addPass(new FixAllFDIVSQRT());
  }
}

void ToyISAV8TargetMachine::anchor() { }

ToyISAV8TargetMachine::ToyISAV8TargetMachine(const Target &T, const Triple &TT,
                                           StringRef CPU, StringRef FS,
                                           const TargetOptions &Options,
                                           std::optional<Reloc::Model> RM,
                                           std::optional<CodeModel::Model> CM,
                                           CodeGenOpt::Level OL, bool JIT)
    : ToyISATargetMachine(T, TT, CPU, FS, Options, RM, CM, OL, JIT, false) {}

void ToyISAV9TargetMachine::anchor() { }

ToyISAV9TargetMachine::ToyISAV9TargetMachine(const Target &T, const Triple &TT,
                                           StringRef CPU, StringRef FS,
                                           const TargetOptions &Options,
                                           std::optional<Reloc::Model> RM,
                                           std::optional<CodeModel::Model> CM,
                                           CodeGenOpt::Level OL, bool JIT)
    : ToyISATargetMachine(T, TT, CPU, FS, Options, RM, CM, OL, JIT, true) {}

void ToyISAelTargetMachine::anchor() {}

ToyISAelTargetMachine::ToyISAelTargetMachine(const Target &T, const Triple &TT,
                                           StringRef CPU, StringRef FS,
                                           const TargetOptions &Options,
                                           std::optional<Reloc::Model> RM,
                                           std::optional<CodeModel::Model> CM,
                                           CodeGenOpt::Level OL, bool JIT)
    : ToyISATargetMachine(T, TT, CPU, FS, Options, RM, CM, OL, JIT, false) {}
