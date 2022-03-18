#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bstypes.h"

#define SAMPLENAMESIZE                  24
#define SAMPLENAMEOFFSET                0
#define SAMPLESIZEOFFSET                24
#define SAMPLEREPEATOFFSET              26
#define SAMPLEREPLENOFFSET              28
#define SAMPLEVOLUMEOFFSET              30

#define SYNTHINDICATOROFFSET    0
#define SYNTHWAVETABLEOFFSET    1
#define SYNTHWAVELENGTHOFFSET   2
#define SYNTHADSRCONTROLOFFSET  4
#define SYNTHADSRTABLEOFFSET    5
#define SYNTHADSRLENGTHOFFSET   6
#define SYNTHADSRSPEEDOFFSET    8
#define SYNTHLFOCONTROLOFFSET   9
#define SYNTHLFOTABLEOFFSET     10
#define SYNTHLFODEPTHOFFSET     11
#define SYNTHLFOLENGTHOFFSET    12
#define SYNTHLFODELAYOFFSET     14
#define SYNTHLFOSPEEDOFFSET     15
#define SYNTHEGCONTROLOFFSET    16
#define SYNTHEGTABLEOFFSET      17
#define SYNTHEGLENGTHOFFSET     18
#define SYNTHEGDELAYOFFSET      20
#define SYNTHEGSPEEDOFFSET      21
#define SYNTHFXCONTROLOFFSET    22
#define SYNTHFXSPEEDOFFSET      23
#define SYNTHFXDELAYOFFSET      24
#define SYNTHMODCONTROLOFFSET   25
#define SYNTHMODTABLEOFFSET     26
#define SYNTHMODSPEEDOFFSET     27
#define SYNTHMODDELAYOFFSET     28
#define SYNTHVOLUMEOFFSET       29
#define SYNTHMODLENGTHOFFSET    30

struct SampledInstr
{
  char name[24];
  WORD size;
  WORD repeat;
  WORD replen;
  WORD volume;
  BYTE* ptr;
};

struct SynthInstr
{
  BYTE indicator;
  BYTE wavetable;
  WORD wavelength;
  BYTE adsrcontrol;
  BYTE adsrtable;
  WORD adsrlength;
  BYTE adsrspeed;
  BYTE lfocontrol;
  BYTE lfotable;
  BYTE lfodepth;
  WORD lfolength;
  BYTE lfodelay;
  BYTE lfospeed;
  BYTE egcontrol;
  BYTE egtable;
  WORD eglength;
  BYTE egdelay;
  BYTE egspeed;
  BYTE fxcontrol;
  BYTE fxspeed;
  BYTE fxdelay;
  BYTE modcontrol;
  BYTE modtable;
  BYTE modspeed;
  BYTE moddelay;
  BYTE volume;
  WORD modlength;
  BYTE* ptr;
};

class Instrument
{
public:
  Instrument();

  void Parse(BYTE ptr[INSTRSIZE], BYTE* memptr, BYTE* starttables);
  bool GetSampledInstr(char *& name, WORD& size, WORD& repeat, WORD& replen, WORD& volume);
  bool GetSynthADSRData(BYTE& table, WORD& size, BYTE& speed);
  bool GetSynthEGData(BYTE& table, WORD& size, BYTE& speed);
  bool GetSynthLFOData(BYTE& table, WORD& size, BYTE& speed, BYTE& depth);
  bool GetSynthFXData(BYTE& table, BYTE& speed);
  bool GetSynthMODData(BYTE table, WORD& size, BYTE& speed);
  bool GetSynthControls(BYTE& adsr, BYTE& lfo, BYTE& eg, BYTE& fx, BYTE& mod);
  bool GetSynthDelays(BYTE& egdelay, BYTE& lfodelay, BYTE& fxdelay, BYTE& moddelay);

  inline WORD GetSampledInstrSize()
  {
    return instrdata.sampledinstr.size;
  }

  inline WORD GetSampledInstrRepeat()
  {
    return instrdata.sampledinstr.repeat;
  }

  inline WORD GetSampledInstrReplen()
  {
    return instrdata.sampledinstr.replen;
  }

  inline WORD GetSampledInstrVolume()
  {
    return instrdata.sampledinstr.volume;
  }

  inline BYTE * GetSampledInstrStart()
  {
    return instrdata.sampledinstr.ptr;
  }

  inline BYTE * GetSynthInstrStart()
  {
    return instrdata.synthinstr.ptr;
  }

  inline WORD GetSynthInstrSize()
  {
    return instrdata.synthinstr.wavelength;
  }

  inline bool IsSynthInstr()
  {
    return issynth;
  }

  inline BYTE GetInstrVolume()
  {
    if(issynth)
      return (BYTE)instrdata.synthinstr.volume;
    else
      return (BYTE)instrdata.sampledinstr.volume;
  }

private:
  bool issynth;
  union InstrData
  {
    SampledInstr sampledinstr;
    SynthInstr synthinstr;
  } instrdata;
};

#endif
