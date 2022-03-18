#include "instrument.h"
#include "memory.h"

Instrument::Instrument()
{
  issynth = false;
}

void Instrument::Parse(BYTE ptr[INSTRSIZE], BYTE* memptr, BYTE* starttables)
{
  if(*(ptr+SYNTHINDICATOROFFSET)==0xFF)
  {       // synth sound
    issynth                          = true;
    instrdata.synthinstr.indicator   = 0xFF;
    instrdata.synthinstr.wavetable   = *(ptr+SYNTHWAVETABLEOFFSET);
    instrdata.synthinstr.wavelength  = 2*(*(ptr+SYNTHWAVELENGTHOFFSET)*256+*(ptr+SYNTHWAVELENGTHOFFSET+1));
    instrdata.synthinstr.adsrcontrol = *(ptr+SYNTHADSRCONTROLOFFSET);
    instrdata.synthinstr.adsrtable   = *(ptr+SYNTHADSRTABLEOFFSET);
    instrdata.synthinstr.adsrlength  = (*(ptr+SYNTHADSRLENGTHOFFSET)*256+*(ptr+SYNTHADSRLENGTHOFFSET+1));
    instrdata.synthinstr.adsrspeed   = *(ptr+SYNTHADSRSPEEDOFFSET);
    instrdata.synthinstr.lfocontrol  = *(ptr+SYNTHLFOCONTROLOFFSET);
    instrdata.synthinstr.lfodelay    = *(ptr+SYNTHLFODELAYOFFSET);
    instrdata.synthinstr.lfodepth    = *(ptr+SYNTHLFODEPTHOFFSET);
    instrdata.synthinstr.lfolength   = (*(ptr+SYNTHLFOLENGTHOFFSET)*256+*(ptr+SYNTHLFOLENGTHOFFSET+1));
    instrdata.synthinstr.lfospeed    = *(ptr+SYNTHLFOSPEEDOFFSET);
    instrdata.synthinstr.lfotable    = *(ptr+SYNTHLFOTABLEOFFSET);
    instrdata.synthinstr.egcontrol   = *(ptr+SYNTHEGCONTROLOFFSET);
    instrdata.synthinstr.egdelay     = *(ptr+SYNTHEGDELAYOFFSET);
    instrdata.synthinstr.eglength    = (*(ptr+SYNTHEGLENGTHOFFSET)*256+*(ptr+SYNTHEGLENGTHOFFSET+1));
    instrdata.synthinstr.egspeed     = *(ptr+SYNTHEGSPEEDOFFSET);
    instrdata.synthinstr.egtable     = *(ptr+SYNTHEGTABLEOFFSET);
    instrdata.synthinstr.fxcontrol   = *(ptr+SYNTHFXCONTROLOFFSET);
    instrdata.synthinstr.fxdelay     = *(ptr+SYNTHFXDELAYOFFSET);
    instrdata.synthinstr.fxspeed     = *(ptr+SYNTHFXSPEEDOFFSET);
    instrdata.synthinstr.modcontrol  = *(ptr+SYNTHMODCONTROLOFFSET);
    instrdata.synthinstr.moddelay    = *(ptr+SYNTHMODDELAYOFFSET);
    instrdata.synthinstr.modspeed    = *(ptr+SYNTHMODSPEEDOFFSET);
    instrdata.synthinstr.modtable    = *(ptr+SYNTHMODTABLEOFFSET);
    instrdata.synthinstr.volume      = *(ptr+SYNTHVOLUMEOFFSET);
    instrdata.synthinstr.modlength   = (*(ptr+SYNTHMODLENGTHOFFSET)*256+*(ptr+SYNTHMODLENGTHOFFSET+1));
    instrdata.synthinstr.ptr         = starttables+TABLESIZE*instrdata.synthinstr.wavetable;
  }
  else
  {       // sample
    issynth = false;
    memcpy(instrdata.sampledinstr.name, ptr+SAMPLENAMEOFFSET, SAMPLENAMESIZE);
    instrdata.sampledinstr.size   = 2*(*(ptr+SAMPLESIZEOFFSET)*256+*(ptr+SAMPLESIZEOFFSET+1));
    instrdata.sampledinstr.repeat = *(ptr+SAMPLEREPEATOFFSET)*256+*(ptr+SAMPLEREPEATOFFSET+1);
    instrdata.sampledinstr.replen = 2*(*(ptr+SAMPLEREPLENOFFSET)*256+*(ptr+SAMPLEREPLENOFFSET+1));
    instrdata.sampledinstr.volume = *(ptr+SAMPLEVOLUMEOFFSET)*256+*(ptr+SAMPLEVOLUMEOFFSET+1);
    instrdata.sampledinstr.ptr    = memptr;
  }
}

bool Instrument::GetSampledInstr(char *& name, WORD& size, WORD& repeat, WORD& replen, WORD& volume)
{
  if(issynth)
    return false;
  name   = instrdata.sampledinstr.name;
  size   = instrdata.sampledinstr.size;
  repeat = instrdata.sampledinstr.repeat;
  replen = instrdata.sampledinstr.replen;
  volume = instrdata.sampledinstr.volume;
  return true;
}


bool Instrument::GetSynthADSRData(BYTE& table, WORD& size, BYTE& speed)
{
  table = instrdata.synthinstr.adsrtable;
  size  = instrdata.synthinstr.adsrlength;
  speed = instrdata.synthinstr.adsrspeed;
  return issynth;
}

bool Instrument::GetSynthEGData(BYTE& table, WORD& size, BYTE& speed)
{
  table = instrdata.synthinstr.egtable;
  size  = instrdata.synthinstr.eglength;
  speed = instrdata.synthinstr.egspeed;
  return issynth;
}

bool Instrument::GetSynthLFOData(BYTE& table, WORD& size, BYTE& speed, BYTE& depth)
{
  table = instrdata.synthinstr.lfotable;
  size  = instrdata.synthinstr.lfolength;
  speed = instrdata.synthinstr.lfospeed;
  depth = instrdata.synthinstr.lfodepth;
  return issynth;
}

bool Instrument::GetSynthFXData(BYTE& table, BYTE& speed)
{
  table = instrdata.synthinstr.wavetable;
  speed = instrdata.synthinstr.fxspeed;
  return issynth;
}

bool Instrument::GetSynthMODData(BYTE table, WORD& size, BYTE& speed)
{
  table = instrdata.synthinstr.modtable;
  size  = instrdata.synthinstr.modlength;
  speed = instrdata.synthinstr.modspeed;
  return issynth;
}

bool Instrument::GetSynthControls(BYTE& adsr, BYTE& lfo, BYTE& eg, BYTE& fx, BYTE& mod)
{
  adsr = instrdata.synthinstr.adsrcontrol;
  eg   = instrdata.synthinstr.egcontrol;
  lfo  = instrdata.synthinstr.lfocontrol;
  fx   = instrdata.synthinstr.fxcontrol;
  mod  = instrdata.synthinstr.modcontrol;
  return issynth;
}

bool Instrument::GetSynthDelays(BYTE& egdelay, BYTE& lfodelay, BYTE& fxdelay, BYTE& moddelay)
{
  egdelay  = instrdata.synthinstr.egdelay;
  lfodelay = instrdata.synthinstr.lfodelay;
  fxdelay  = instrdata.synthinstr.fxdelay;
  moddelay = instrdata.synthinstr.moddelay;
  return issynth;
}
