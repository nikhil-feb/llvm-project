//===-- ToyISATargetInfo.cpp - ToyISA Target Implementation -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TargetInfo/ToyISATargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
using namespace llvm;

Target &llvm::getTheToyISATarget() {
  static Target TheToyISATarget;
  return TheToyISATarget;
}
Target &llvm::getTheToyISAV9Target() {
  static Target TheToyISAV9Target;
  return TheToyISAV9Target;
}
Target &llvm::getTheToyISAelTarget() {
  static Target TheToyISAelTarget;
  return TheToyISAelTarget;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeToyISATargetInfo() {
  RegisterTarget<Triple::ToyISA, /*HasJIT=*/false> X(getTheToyISATarget(),
                                                    "ToyISA", "ToyISA", "ToyISA");
  RegisterTarget<Triple::ToyISAv9, /*HasJIT=*/false> Y(
      getTheToyISAV9Target(), "ToyISAv9", "ToyISA V9", "ToyISA");
  RegisterTarget<Triple::ToyISAel, /*HasJIT=*/false> Z(
      getTheToyISAelTarget(), "ToyISAel", "ToyISA LE", "ToyISA");
}
