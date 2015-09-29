#ifndef EIGHTUNE_H_
#define EIGHTUNE_H_

#ifdef __cplusplus
extern "C" {
#endif

//Data types
typedef unsigned char u8;      // unsigned u8 (8bit)
typedef unsigned short int u16;// unsigned u16 (16bit)

//Zeroes the DPCM table (not necessary)
void eightune_resetDPCM();

//Inits the synth
void eightune_init();

//Synthesizes and mixed the 4 channels
u8 eightune_synth(unsigned long t);

//Chip WR function
void eightune_writeData(u8 addr, u8 data);
//TODO: Chip RD function

#ifdef __cplusplus
}
#endif

#endif /* EIGHTUNE_H_ */
