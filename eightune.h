//Data types
typedef unsigned char byte;
typedef unsigned short int word;

//Waveforms 
#include "wv_square50.h"
#include "wv_square25.h"
#include "wv_square12.h"
#include "wv_triangle.h"
#include "wv_sine.h"
#include "wv_sawtooth.h"
#include "wv_noise.h"
//DPCM waveform Table
byte dpcm[240];

byte* waveforms[] = {&square50, &square25, &square12, &triangle, &sine, &sawtooth, &noise, &dpcm};

//Channels
struct Channel {
  word freq;        //16bit frequency register
  byte* waveform;   //Waveform array pointer
  byte volume;      //5bits of volume (upper 3 bit don't count)
} a, b, c ,d;

Channel* chans[] = {&a, &b, &c, &d};


//Zeroes the DPCM table
void resetDPCM() {
  for(byte i=0; i<240; i++)
    dpcm[i] = 0;
}

//Synthesizes and mixed the 4 channels
byte synth(long t) {
  //TODO: make compatible with DPCM
  byte aout = ( a.waveform[ (t * a.freq) % 256 ] ) >> (a.volume + 2);   //the +2 is a division by 4, to mix the channels
  byte bout = ( b.waveform[ (t * a.freq) % 256 ] ) >> (b.volume + 2); 
  byte cout = ( c.waveform[ (t * a.freq) % 256 ] ) >> (c.volume + 2); 
  byte dout = ( d.waveform[ (t * a.freq) % 256 ] ) >> (d.volume + 2);
  
  return aout + bout + cout + dout;
}

inline void writeData(byte addr, byte data) {
  if(addr < 0x10) // channel data
  {
    Channel* chan;
    
    //Select the channel
    chan = chans[addr / 3];
    
    //Write to register
    switch(addr % 3) {
      case 0:  //low frequency byte
      {
        word high = chan->freq & 0xF0;
        chan->freq = high | data;
        break;
      }
        
      case 1:  //high frequency byte
      {
        word low = chan->freq & 0x0F;
        chan->freq = low | data;
        break;
      }
        
      case 2:  //flags
      {
        //Volume
        chan->volume = 31 - (data >> 3);
        
        //Waveform
        chan->waveform = waveforms[data & 0x7];
        break;
      }
    }
  }
  else  // DPCM table
    dpcm[addr - 0x10] = data;
}
