#include "module.h"
#include <memory.h>

Module::Module(BYTE* ptr, size_t size)
{
  modptr = new BYTE[size];
  memcpy(modptr, ptr, size);

  numpatterns      = 0;
  numsteps         = 0;
  numtables        = 0;
  samplesconverted = false;
}

Module::~Module()
{
  delete[] modptr;
}

bool Module::Init()
{
  WORD  pattern;
  BYTE* ptr, * sampptr;
  WORD  maxpattern = 0;

  if(memcmp(modptr+26, "BPSM", 4)==0)
    memcpy(modptr+26, "V.3\0", 4);
  if(memcmp(modptr+26, "V.2", 3)==0)
    ConvertHeader();

  memcpy(songname, modptr, 26);
  songname[26] = 0;
  if(memcmp(modptr+26, "V.3", 3)!=0)
    return false;
  numtables     = (WORD)*(modptr+29);
  numsteps      = (WORD)(*(modptr+30)*256+*(modptr+31));
  startsteps    = modptr+HEADERSIZE;
  startpatterns = startsteps+STEPSIZE*numsteps;

  ptr = startsteps;
  for(WORD i = 0; i<numsteps*VOICES; i++)
  {
    pattern = 256*(*ptr)+*(ptr+1);
    if(maxpattern<pattern)
      maxpattern = pattern;
    ptr += 4;
  }
  starttables  = startpatterns+PATTERNSIZE*maxpattern;
  startsamples = starttables+TABLESIZE*numtables;
  if(numtables!=0 && !samplesconverted)
    ConvertAmigaSample(starttables, numtables*TABLESIZE);
  ptr     = modptr+SONGDATASIZE;
  sampptr = startsamples;
  for(WORD i = 0; i<NUMINSTR; i++)
  {
    instr[i].Parse(ptr, sampptr, starttables);
    if(!instr[i].IsSynthInstr())
    {
      if(!samplesconverted)
        ConvertAmigaSample(sampptr, instr[i].GetSampledInstrSize());
      sampptr += instr[i].GetSampledInstrSize();
    }
    ptr += INSTRSIZE;
  }
  samplesconverted = true;
  return true;
}

WORD Module::GetPattern(WORD step, WORD voice)
{
  BYTE* ptr = startsteps+STEPVOICESIZE*voice+step*STEPSIZE;
  return 256*(*ptr)+*(ptr+1);
}

bool Module::GetSampledInstrument(short instr, char *& name, WORD& size,  WORD& repeat, WORD& replen, WORD& volume)
{
  if(instr>NUMINSTR || instr<1 || IsSynthInstrument(instr))
    return false;
  return this->instr[instr-1].GetSampledInstr(name, size, repeat, replen, volume);
}

bool Module::GetSampledInstrument(short instr, WORD& size, WORD& repeat, WORD& replen, WORD& volume, BYTE *& start)
{
  char* dummy;
  if(instr>NUMINSTR || instr<1 || IsSynthInstrument(instr))
    return false;
  start = this->instr[instr-1].GetSampledInstrStart();
  return this->instr[instr-1].GetSampledInstr(dummy, size, repeat, replen, volume);
}

bool Module::GetSynthInstrument(short instr, WORD& size, BYTE *& start)
{
  if(instr>NUMINSTR || instr<1 || !IsSynthInstrument(instr))
    return false;
  start = this->instr[instr-1].GetSynthInstrStart();
  size  = this->instr[instr-1].GetSynthInstrSize();
  return true;
}

bool Module::GetSynthDelays(short instr, BYTE& egdelay, BYTE& lfodelay, BYTE& fxdelay, BYTE& moddelay)
{
  if(instr>NUMINSTR || instr<1 || !IsSynthInstrument(instr))
    return false;
  return this->instr[instr-1].GetSynthDelays(egdelay, lfodelay, fxdelay, moddelay);
}

bool Module::GetSynthEGData(short instr, BYTE& table, WORD& size, BYTE& speed)
{
  if(instr>NUMINSTR || instr<1 || !IsSynthInstrument(instr))
    return false;
  return this->instr[instr-1].GetSynthEGData(table, size, speed);
}

bool Module::GetSynthLFOData(short instr, BYTE& table, WORD& size, BYTE& speed, BYTE& depth)
{
  if(instr>NUMINSTR || instr<1 || !IsSynthInstrument(instr))
    return false;
  return this->instr[instr-1].GetSynthLFOData(table, size, speed, depth);
}

bool Module::GetSynthFXData(short instr, BYTE& table, BYTE& speed)
{
  if(instr>NUMINSTR || instr<1 || !IsSynthInstrument(instr))
    return false;
  return this->instr[instr-1].GetSynthFXData(table, speed);
}

bool Module::GetSynthMODData(short instr, BYTE table, WORD& size, BYTE& speed)
{
  if(instr>NUMINSTR || instr<1 || !IsSynthInstrument(instr))
    return false;
  return this->instr[instr-1].GetSynthMODData(table, size, speed);
}

bool Module::GetSynthADSRData(short instr, BYTE& table, WORD& size, BYTE& speed)
{
  if(instr>NUMINSTR || instr<1 || !IsSynthInstrument(instr))
    return false;
  return this->instr[instr-1].GetSynthADSRData(table, size, speed);
}

bool Module::GetSynthControls(short instr, BYTE& adsr, BYTE& lfo, BYTE& eg, BYTE& fx, BYTE& mod)
{
  if(instr>NUMINSTR || instr<1 || !IsSynthInstrument(instr))
    return false;
  return this->instr[instr-1].GetSynthControls(adsr, lfo, eg, fx, mod);
}

void Module::ConvertAmigaSample(BYTE* ptr, int size)
{
  while(size>0)
  {
    (*ptr) += 128;
    ptr++;
    size--;
  }
}

void Module::ConvertHeader()
{
  BYTE* ptr = modptr+SONGDATASIZE;
  memcpy(modptr+26, "V.3", 3);
  for(short i = 0; i<NUMINSTR; i++)
  {
    if(ptr[0]==0xFF)
    {
      ptr[31] = 0;
      ptr[30] = 0;
      ptr[29] = ptr[25];
      ptr[28] = 0;
      ptr[27] = 1;
      ptr[26] = 0;
      ptr[25] = 0;
      ptr[14] = ptr[15];
      ptr[15] = ptr[16];
      ptr[16] = ptr[17];
      ptr[17] = ptr[18];
      ptr[18] = ptr[20];
      ptr[19] = ptr[21];
      ptr[20] = ptr[23];
      ptr[21] = ptr[24];
      ptr[22] = 0;
      ptr[23] = 1;
      ptr[24] = 0;
    }
    ptr += INSTRSIZE;
  }
}
