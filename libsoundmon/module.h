#ifndef MODULE_H
#define MODULE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bstypes.h"
#include "instrument.h"

class Module
{
public:
  Module(BYTE* ptr, size_t size);
  ~Module();

  bool Init();

  WORD GetPattern(WORD step, WORD voice);

  inline BYTE GetST(WORD step, WORD voice)
  {
    return *(startsteps+STEPVOICESIZE*voice+step*STEPSIZE+2);
  }

  inline BYTE GetTR(WORD step, WORD voice)
  {
    return *(startsteps+STEPVOICESIZE*voice+step*STEPSIZE+3);
  }

  inline BYTE GetNote(WORD pattern, WORD position)
  {
    return *(startpatterns+PATTERNSIZE*(pattern-1)+position*NOTESIZE);
  }

  inline BYTE GetFX(WORD pattern, WORD position)
  {
    return (*(startpatterns+PATTERNSIZE*(pattern-1)+position*NOTESIZE+1))&0x0F;
  }

  inline BYTE GetInstrument(WORD pattern, WORD position)
  {
    return ((*(startpatterns+PATTERNSIZE*(pattern-1)+position*NOTESIZE+1))&0xF0)>>4;
  }

  inline BYTE GetFXByte(WORD pattern, WORD position)
  {
    return (*(startpatterns+PATTERNSIZE*(pattern-1)+position*NOTESIZE+2));
  }

  inline const char * GetSongName()
  {
    return songname;
  }

  inline WORD GetNumTables()
  {
    return numtables;
  }

  inline WORD GetNumPatterns()
  {
    return numpatterns;
  }

  inline WORD GetNumSteps()
  {
    return numsteps;
  }

  inline BYTE GetInstrumentVolume(short instr)
  {
    if(instr>NUMINSTR || instr<1)
      return 0;
    else
      return this->instr[instr-1].GetInstrVolume();
  }

  inline bool IsSynthInstrument(short instr)
  {
    if(instr>NUMINSTR || instr<1)
      return false;
    else
      return this->instr[instr-1].IsSynthInstr();
  }

  inline BYTE GetTableValue(BYTE table, WORD offset)
  {
    return *(starttables+int(table)*TABLESIZE+offset);
  }

  bool GetSampledInstrument(short instr, char *& name, WORD& size, WORD& repeat, WORD& replen, WORD& volume);
  bool GetSampledInstrument(short instr, WORD& size, WORD& repeat, WORD& replen, WORD& volume, BYTE *& start);
  bool GetSynthInstrument(short instr, WORD& size, BYTE *& start);
  bool GetSynthDelays(short instr, BYTE& egdelay, BYTE& lfodelay, BYTE& fxdelay, BYTE& moddelay);
  bool GetSynthEGData(short instr, BYTE& table, WORD& size, BYTE& speed);
  bool GetSynthLFOData(short instr, BYTE& table, WORD& size, BYTE& speed, BYTE& depth);
  bool GetSynthFXData(short instr, BYTE& table, BYTE& speed);
  bool GetSynthMODData(short instr, BYTE table, WORD& size, BYTE& speed);
  bool GetSynthADSRData(short instr, BYTE& table, WORD& size, BYTE& speed);
  bool GetSynthControls(short instr, BYTE& adsr, BYTE& lfo, BYTE& eg, BYTE& fx, BYTE& mod);

private:
  BYTE*      modptr;
  BYTE*      startsteps;
  BYTE*      startpatterns;
  BYTE*      starttables;
  BYTE*      startsamples;
  char       songname[27];
  WORD       numsteps;
  WORD       numpatterns;
  WORD       numtables;
  Instrument instr[NUMINSTR];
  bool       samplesconverted;

  void ConvertAmigaSample(BYTE* ptr, int size);
  void ConvertHeader();
};

#endif
