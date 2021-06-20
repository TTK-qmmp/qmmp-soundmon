#include "channel.h"

Channel::Channel()
{
  memptr      = NULL;
  size        = 0;
  repeat      = 0;
  replen      = 0;
  volume      = 0;
  leftchannel = true;
  playcursor  = 0;
  altmemptr   = NULL;
  offset      = 0;
  numofbytes  = 0;
  count       = 0;
  playing     = false;
}

void Channel::SetPlayBuffer(BYTE* memptr, WORD size, WORD repeat, WORD replen)
{
  this->memptr     = memptr;
  this->size       = size;
  this->repeat     = repeat;
  this->replen     = replen;
  this->playcursor = 0;
  this->altmemptr  = NULL;
  this->offset     = 0;
  this->numofbytes = 0;
  count            = 0;
}

void Channel::GetNextBytes(BYTE& left, BYTE& right)
{
  int newval;
  left  = 128;
  right = 128;

  if(!playing || memptr==NULL)
  {
    return;
  }
  if(numofbytes!=0 && playcursor>=offset && playcursor<=(offset+numofbytes))
  { // updated waveform
    newval = *(altmemptr+playcursor-offset);
  }
  else
  {
    newval = *(memptr+playcursor);
  }
  newval = ((newval-128)*volume)>>6; // 64
  if(leftchannel)
    left = newval+128;
  else
    right = newval+128;
  count++;
  if(count>=freqrepeat)
  {
    count = count-freqrepeat;
    playcursor++;
    if(replen>2)
    {
      if(playcursor>=(repeat+replen))
        playcursor = repeat;  // for repeating samples
    }
    else
    {
      if(playcursor>=size)    // non repeating
        playing = false;
    }
  }
}

void Channel::Update(BYTE* memptr, WORD offset, WORD numofbytes)
{
  this->altmemptr  = memptr;
  this->offset     = offset;
  this->numofbytes = numofbytes;
}
