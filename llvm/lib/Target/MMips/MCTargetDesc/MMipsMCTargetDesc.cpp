#include "MMipsMCTargetDesc.h"
#include "InstPrinter/MMipsInstPrinter.h"
#include "MMipsMCAsmInfo.h"
#include "llvm/MC/MCCodeGenInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/TargetRegistry.h"
#define GET_INSTRINFO_MC_DESC
#include "MMipsGenInstrInfo.inc"
#define GET_SUBTARGETINFO_MC_DESC
#include "MMipsGenSubtargetInfo.inc"
#define GET_REGINFO_MC_DESC
#include "MMipsGenRegisterInfo.inc"
using namespace llvm;
static MCInstrInfo *createMMipsMCInstrInfo() {
MCInstrInfo *X = new MCInstrInfo();
InitMMipsMCInstrInfo(X);
return X;
}
static MCRegisterInfo *createMMipsMCRegisterInfo(StringRef
TT) {
MCRegisterInfo *X = new MCRegisterInfo();
InitMMipsMCRegisterInfo(X, MMips::LR);
return X;
}
static MCSubtargetInfo *createMMipsMCSubtargetInfo(StringRef
TT, StringRef CPU,
StringRef
FS) {
MCSubtargetInfo *X = new MCSubtargetInfo();
InitMMipsMCSubtargetInfo(X, TT, CPU, FS);
return X;
}
static MCAsmInfo *createMMipsMCAsmInfo(const MCRegisterInfo
&MRI, StringRef TT) {
MCAsmInfo *MAI = new MMipsMCAsmInfo(TT);
return MAI;
}
static MCCodeGenInfo *createMMipsMCCodeGenInfo(StringRef TT,
Reloc::Model RM,
CodeModel::Model CM,
CodeGenOpt::Level OL)
{
MCCodeGenInfo *X = new MCCodeGenInfo();
if (RM == Reloc::Default) {
RM = Reloc::Static;
}
if (CM == CodeModel::Default) {
CM = CodeModel::Small;
}
if (CM != CodeModel::Small && CM != CodeModel::Large) {
report_fatal_error("Target only supports CodeModel
Small or Large");
}
X->InitMCCodeGenInfo(RM, CM, OL);
return X;
}
static MCInstPrinter *
createMMipsMCInstPrinter(const Target &T, unsigned
SyntaxVariant,
const MCAsmInfo &MAI, const
MCInstrInfo &MII,
const MCRegisterInfo &MRI, const
MCSubtargetInfo &STI) {
return new MMipsInstPrinter(MAI, MII, MRI);
}
static MCStreamer *
createMCAsmStreamer(MCContext &Ctx, formatted_raw_ostream
&OS, bool isVerboseAsm, bool useDwarfDirectory,MCInstPrinter
*InstPrint, MCCodeEmitter *CE,MCAsmBackend *TAB, bool ShowInst) {
return createAsmStreamer(Ctx, OS, isVerboseAsm,
useDwarfDirectory, InstPrint, CE, TAB, ShowInst);
}
static MCStreamer *createMCStreamer(const Target &T,
StringRef TT,
MCContext &Ctx,
MCAsmBackend &MAB,
raw_ostream &OS,
MCCodeEmitter *Emitter,
const MCSubtargetInfo
&STI,
bool RelaxAll,
bool NoExecStack) {
return createELFStreamer(Ctx, MAB, OS, Emitter, false,
NoExecStack);
}
// Force static initialization.
extern "C" void LLVMInitializeMMipsTargetMC() {
// Register the MC asm info.
RegisterMCAsmInfoFn X(TheMMipsTarget, createMMipsMCAsmInfo);
// Register the MC codegen info.
TargetRegistry::RegisterMCCodeGenInfo(TheMMipsTarget,
createMMipsMCCodeGenInfo);
// Register the MC instruction info.
TargetRegistry::RegisterMCInstrInfo(TheMMipsTarget,
createMMipsMCInstrInfo);
// Register the MC register info.
TargetRegistry::RegisterMCRegInfo(TheMMipsTarget,
createMMipsMCRegisterInfo);
// Register the MC subtarget info.
TargetRegistry::RegisterMCSubtargetInfo(TheMMipsTarget,
createMMipsMCSubtargetInfo);
// Register the MCInstPrinter
TargetRegistry::RegisterMCInstPrinter(TheMMipsTarget,
createMMipsMCInstPrinter);
// Register the ASM Backend.
TargetRegistry::RegisterMCAsmBackend(TheMMipsTarget,
createMMipsAsmBackend);
// Register the assembly streamer.
TargetRegistry::RegisterAsmStreamer(TheMMipsTarget,
createMCAsmStreamer);
// Register the object streamer.
TargetRegistry::RegisterMCObjectStreamer(TheMMipsTarget,
createMCStreamer);
// Register the MCCodeEmitter
TargetRegistry::RegisterMCCodeEmitter(TheMMipsTarget,
createMMipsMCCodeEmitter);
}