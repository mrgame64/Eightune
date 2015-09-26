#include <Arduino.h>

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

//Channels
struct Channel {
  word freq;        //16bit frequency register
  byte* waveform;   //Waveform array pointer
  byte volume;      //5bits of volume (upper 3 bit don't count)
} a, b, c ,d;


//Zeroes the DPCM table
void resetDPCM() {
  for(byte i=0; i<240; i++)
    dpcm[i] = 0;
}

//Synthesizes and mixed the 4 channels
byte synth() {
  //TODO: faster timing
  long t = micros();
  
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
    //TODO: optimize with a table
    switch(addr / 3) {
      case 0: chan = &a; break;
      case 1: chan = &b; break;
      case 2: chan = &c; break;
      case 3: chan = &d; break;
    }
    
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
        //TODO: optimize with a table
        switch(data & 0x7) {
          case 0: chan->waveform = &wv_square50; break;
          case 1: chan->waveform = &wv_square25; break;
          case 2: chan->waveform = &wv_square12; break;
          case 3: chan->waveform = &wv_triangle; break;
          case 4: chan->waveform = &wv_sine; break;
          case 5: chan->waveform = &wv_sawtooth; break;
          case 6: chan->waveform = &wv_noise; break;
          case 7: chan->waveform = &dpcm; break;
        }
        break;
      }
    }
  }
  else  // DPCM table
    dpcm[addr - 0x10] = data;
}
