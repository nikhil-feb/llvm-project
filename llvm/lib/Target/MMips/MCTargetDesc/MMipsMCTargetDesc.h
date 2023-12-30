#ifndef MMipsMCTARGETDESC_H
#define MMipsMCTARGETDESC_H
#include "llvm/Support/DataTypes.h"
namespace llvm {
class Target;
class MCInstrInfo;
class MCRegisterInfo;
class MCSubtargetInfo;
class MCContext;
class MCCodeEmitter;
class MCAsmInfo;
class MCCodeGenInfo;
class MCInstPrinter;
class MCObjectWriter;
class MCAsmBackend;
class StringRef;
class raw_ostream;
extern Target TheMMipsTarget;
MCCodeEmitter *createMMipsMCCodeEmitter(const MCInstrInfo &MCII,
const MCRegisterInfo &MRI, const MCSubtargetInfo &STI, MCContext
&Ctx);
MCAsmBackend *createMMipsAsmBackend(const Target &T, const
MCRegisterInfo &MRI, StringRef TT, StringRef CPU);
MCObjectWriter *createMMipsELFObjectWriter(raw_ostream &OS,
uint8_t OSABI);
} // End llvm namespace
#define GET_REGINFO_ENUM
#include "MMipsGenRegisterInfo.inc"
#define GET_INSTRINFO_ENUM
#include "MMipsGenInstrInfo.inc"
#define GET_SUBTARGETINFO_ENUM
#include "MMipsGenSubtargetInfo.inc"
#endif