//Data types
typedef unsigned char u8;      // unsigned u8 (8bit)
typedef unsigned short int u16;// unsigned u16 (16bit)

//Waveforms 
#include "wv_square50.h"
#include "wv_square25.h"
#include "wv_square12.h"
#include "wv_triangle.h"
#include "wv_sine.h"
#include "wv_sawtooth.h"
#include "wv_noise.h"
//DPCM waveform Table
u8 dpcm[240];

//Channels
struct Channel {
  u16 freq;        //16bit frequency register
  const u8* waveform;   //Waveform array pointer
  u8 volume;      //5bits of volume (upper 3 bit don't count)
} a, b, c ,d;

Channel* chans[] = {&a, &b, &c, &d};


//Zeroes the DPCM table
void resetDPCM() {
  for(u8 i = 0; i < 240; i++)
    dpcm[i] = 0;
}

//Synthesizes and mixed the 4 channels
u8 synth(long t) {
  //TODO: make compatible with DPCM
  u8 aout = ( a.waveform[ (t * a.freq) % 256 ] ) >> (a.volume + 2);   //the +2 is a division by 4, to mix the channels
  u8 bout = ( b.waveform[ (t * a.freq) % 256 ] ) >> (b.volume + 2); 
  u8 cout = ( c.waveform[ (t * a.freq) % 256 ] ) >> (c.volume + 2); 
  u8 dout = ( d.waveform[ (t * a.freq) % 256 ] ) >> (d.volume + 2);
  
  return aout + bout + cout + dout;
}

inline void writeData(u8 addr, u8 data) {
  if(addr < 0x10) // channel data
  {
    Channel* chan;
    
    //Select the channel
    chan = chans[addr / 3];
    
    //Write to register
    switch(addr % 3) {
      case 0:  //low frequency u8
      {
        u16 high = chan->freq & 0xF0;
        chan->freq = high | data;
        break;
      }
        
      case 1:  //high frequency u8
      {
        u16 low = chan->freq & 0x0F;
        chan->freq = low | data;
        break;
      }
        
      case 2:  //flags
      {
        //Volume
        chan->volume = 31 - (data >> 3);
        
        //Waveform
        switch(data & 0x7) {
          case 0: chan->waveform = square50; break;
          case 1: chan->waveform = square25; break;
          case 2: chan->waveform = square12; break;
          case 3: chan->waveform = triangle; break;
          case 4: chan->waveform = sine; break;
          case 5: chan->waveform = sawtooth; break;
          case 6: chan->waveform = noise; break;
          case 7: chan->waveform = dpcm; break;
        }
        break;
      }
    }
  }
  else  // DPCM table
    dpcm[addr - 0x10] = data;
}
