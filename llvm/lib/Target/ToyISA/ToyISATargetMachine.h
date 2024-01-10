//===-- ToyISATargetMachine.h - Define TargetMachine for ToyISA ---*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares the ToyISA specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_ToyISA_ToyISATARGETMACHINE_H
#define LLVM_LIB_TARGET_ToyISA_ToyISATARGETMACHINE_H

#include "ToyISAInstrInfo.h"
#include "ToyISASubtarget.h"
#include "llvm/Target/TargetMachine.h"
#include <optional>

namespace llvm {

class ToyISATargetMachine : public LLVMTargetMachine {
  std::unique_ptr<TargetLoweringObjectFile> TLOF;
  ToyISASubtarget Subtarget;
  bool is64Bit;
  mutable StringMap<std::unique_ptr<ToyISASubtarget>> SubtargetMap;

public:
  ToyISATargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                     StringRef FS, const TargetOptions &Options,
                     std::optional<Reloc::Model> RM,
                     std::optional<CodeModel::Model> CM, CodeGenOpt::Level OL,
                     bool JIT, bool is64bit);
  ~ToyISATargetMachine() override;

  const ToyISASubtarget *getSubtargetImpl() const { return &Subtarget; }
  const ToyISASubtarget *getSubtargetImpl(const Function &) const override;

  // Pass Pipeline Configuration
  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;
  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }

  MachineFunctionInfo *
  createMachineFunctionInfo(BumpPtrAllocator &Allocator, const Function &F,
                            const TargetSubtargetInfo *STI) const override;
};

/// ToyISA 32-bit target machine
///
class ToyISAV8TargetMachine : public ToyISATargetMachine {
  virtual void anchor();

public:
  ToyISAV8TargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                       StringRef FS, const TargetOptions &Options,
                       std::optional<Reloc::Model> RM,
                       std::optional<CodeModel::Model> CM, CodeGenOpt::Level OL,
                       bool JIT);
};

/// ToyISA 64-bit target machine
///
class ToyISAV9TargetMachine : public ToyISATargetMachine {
  virtual void anchor();

public:
  ToyISAV9TargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                       StringRef FS, const TargetOptions &Options,
                       std::optional<Reloc::Model> RM,
                       std::optional<CodeModel::Model> CM, CodeGenOpt::Level OL,
                       bool JIT);
};

class ToyISAelTargetMachine : public ToyISATargetMachine {
  virtual void anchor();

public:
  ToyISAelTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                       StringRef FS, const TargetOptions &Options,
                       std::optional<Reloc::Model> RM,
                       std::optional<CodeModel::Model> CM, CodeGenOpt::Level OL,
                       bool JIT);
};

} // end namespace llvm

#endif
