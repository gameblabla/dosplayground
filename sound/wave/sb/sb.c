/****************************************************************************
** WARNING: You need a COMPACT memory model to run this.                   **
*****************************************************************************
** Demonstration of playing a single wave file using DMA                   **
**  by Steven H Don                                                        **
**                                                                         **
** For questions, feel free to e-mail me.                                  **
**                                                                         **
**    shd@earthling.net                                                    **
**    http://shd.cjb.net                                                   **
**                                                                         **
****************************************************************************/
//Include files
#include <string.h>
#include <malloc.h>
#include <dos.h>
#include <conio.h>
#include <stdio.h>
#include "sound.h"

struct HeaderType {
  long         RIFF;      //RIFF header
  char         NI1 [4];   //not important
  long         WAVE;      //WAVE identifier
  long         fmt;       //fmt  identifier
  char         NI2 [6];   //not important
  unsigned int Channels;  //channels 1 = mono; 2 = stereo
  long         Frequency; //sample frequency
  char         NI3 [6];   //not important
  unsigned int BitRes;    //bit resolution 8/16 bit
  long         data;      //data identifier
  long         datasize;  //size of data chunk
};
struct HeaderType Header;

unsigned int    Base;          //Sound Blaster base address

/****************************************************************************
** Checks to see if a Sound Blaster exists at a given address, returns     **
** true if Sound Blaster found, false if not.                              **
****************************************************************************/
char ResetDSP(unsigned int Test)
{
  //Reset the DSP
  outp (Test + 0x6, 1);
  delay(10);
  outp (Test + 0x6, 0);
  delay(10);
  //Check if reset was succesfull
  if (((inp(Test + 0xE) & 0x80) == 0x80) && (inp (Test + 0xA) == 0xAA)) {
    //DSP was found
    Base = Test;
    return (1);
  } else
    //No DSP was found
    return (0);
}

/****************************************************************************
** Send a byte to the DSP (Digital Signal Processor) on the Sound Blaster  **
****************************************************************************/
void WriteDSP(unsigned char Value)
{
	//Wait for the DSP to be ready to accept data
	// Thanks Geri for fix
	// "If the sb freezes, the system will not freeze"
	int hibaw=0;
    while((inp(Base+0xC)&0x80)==0x80)
    {
		hibaw++;
		if(hibaw>65534) break;
    }
	//Send byte
	outp (Base + 0xC, Value);
}

/****************************************************************************
** Plays a part of the memory                                              **
****************************************************************************/
void PlayBack (struct WaveData *Wave)
{
  long          LinearAddress;
  unsigned int  Page, OffSet;
  unsigned char TimeConstant;

  TimeConstant = (65536 - (256000000 / Wave->Frequency)) >> 8;
  WriteDSP(0x40);                  //DSP-command 40h - Set sample frequency
  WriteDSP(TimeConstant);          //Write time constant

  //Convert pointer to linear address
  LinearAddress = FP_SEG (Wave->Sample);
  LinearAddress = (LinearAddress << 4) + FP_OFF (Wave->Sample);
  Page = LinearAddress >> 16;      //Calculate page
  OffSet = LinearAddress & 0xFFFF; //Calculate offset in the page
  /*
      Note - this procedure only works with DMA channel 1
  */
  outp (0x0A, 5);              //Mask DMA channel 1
  outp (0x0C, 0);              //Clear byte pointer
  outp (0x0B, 0x49);           //Set mode
  /*
      The mode consists of the following:
      0x49 = binary 01 00 10 01
                    |  |  |  |
                    |  |  |  +- DMA channel 01
                    |  |  +---- Read operation (the DSP reads from memory)
                    |  +------- Single cycle mode
                    +---------- Block mode
  */

  outp (0x02, OffSet & 0xFF); //Write the offset to the DMA controller
  outp (0x02, OffSet >> 8);

  outp (0x83, Page);           //Write the page to the DMA controller

  outp (0x03, Wave->SoundLength & 0xFF);
  outp (0x03, Wave->SoundLength >> 8);

  outp (0x0A, 1);              //Unmask DMA channel

  WriteDSP (0xD1);                 //Enable speaker

  WriteDSP (0x14);                 //DSP-command 14h - Single cycle playback
  WriteDSP(Wave->SoundLength & 0xFF);
  WriteDSP(Wave->SoundLength >> 8);
}

/****************************************************************************
** Loads a wave file into memory.                                          **
** This procedure expects a _very_ standard wave header.                   **
** It doesn't perform much error checking.                                 **
****************************************************************************/
int LoadVoice (struct WaveData *Voice, const char *FileName)
{
  FILE *WAVFile;

  //If it can't be opened...
  WAVFile = fopen(FileName, "rb");
  if (WAVFile == NULL) {
    //..display error message
    //printf ("Unable to open wave file\n");
    return (0);
  }

  //Load the sample data
  fread(&Header, sizeof (Header), 1, WAVFile);

  //Check RIFF, WAVE, fmt and data headers
  if ((Header.RIFF != 0x46464952) ||
      (Header.WAVE != 0x45564157) ||
      (Header.fmt  != 0x20746D66) ||
      (Header.data != 0x61746164)) {
    //printf ("Not a wave file\n");
    return (0);
  }

  //Check channels
  if (Header.Channels != 1) {
    //printf ("Not a mono wave file\n");
    return (0);
  }

  //Check bit resolution
  if (Header.BitRes != 8) {
    //printf ("Not an 8-bit wave file\n");
    return (0);
  }

  //Return length of file
  Voice->SoundLength = Header.datasize;

  //Check size
  if (Voice->SoundLength > 32000) {
    //printf ("Warning: sample size is more than 32K, this may cause problems with EMM386\n");
    //printf ("Press ESCAPE to quit, or any other key to ignore.\n");
    if (getch() == 27) {
      return (0);
    }
    if (Voice->SoundLength > 64000) {
      //printf ("File to large, only first part will load and play.\n");
      Voice->SoundLength = 64000;
    }
  }

  Voice->Frequency = Header.Frequency;

  //Assign memory
  Voice->Sample = (char *)malloc(Voice->SoundLength);
  if (!Voice->Sample) {
    //printf ("Memory allocation error\n");
    return (0);
  }

  //Load the sample data
  fread(Voice->Sample, Voice->SoundLength, 1, WAVFile);

  fclose (WAVFile); //Close the file

  return (1);
}

void SB_Init()
{
	if (ResetDSP (0x220))
	{
		//printf ("Sound Blaster found at 220h\n");
	}
	else
	{
		if (ResetDSP (0x240))
		{
			//printf ("Sound Blaster found at 240h\n");
		}
		else
		{
			//or none at all
			//printf ("No Sound Blaster found\n");
			return;
		}
	}
}


void SB_Load(const char* file, struct WaveData *vc)
{
	LoadVoice (vc, file);
}

void SB_Unload(struct WaveData *vc)
{
	if (vc->Sample)
	{
		free(vc->Sample);
	}
}

void SB_Play(struct WaveData *vc)
{
	PlayBack (vc);
}

void SB_Close()
{
	WriteDSP (0xD0);
}


SoundDevice SB_device = {
	"SB driver",
	SB_Init,
	SB_Load,
	SB_Play,
	SB_Unload,
	SB_Close
};
