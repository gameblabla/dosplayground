/****************************************************************************
** Demonstration of playing a single wave file through DMA using DJGPP     **
**  by Steven H Don                                                        **
**                                                                         **
** For questions, feel free to e-mail me.                                  **
**                                                                         **
**    shd@earthling.net                                                    **
**    http://shd.cjb.net                                                   **
**                                                                         **
****************************************************************************/
//Include files
#include <conio.h>
#include <stdio.h>
#include <dos.h>
#include <malloc.h>
#include <math.h>
#include <mem.h>
#include <io.h>

#include <stdlib.h>
#include <string.h>
#ifdef DJGPP
#include <go32.h>
#include <dpmi.h>
#include <sys/movedata.h>
#include <sys/nearptr.h>
#include <pc.h>
#endif

#include "sound.h"

static char                          Key;
static unsigned int              CBuffer; //Clear Buffer indicator
static unsigned char          *DMABuffer; //Pointer to protected mode DMA Buffer
static unsigned int                  DMA; //The DMA channel
static unsigned int                  IRQ; //The IRQ level
static unsigned int                 Base; //Sound Blaster base address, Word

#ifdef DJGPP
//Pointers to old and new interrupt routines
static _go32_dpmi_seginfo  OldIRQ, MyIRQ;

//Pointer to DOS DMA buffer
static _go32_dpmi_seginfo         DOSBuf;
static int                     DOSBufOfs;
#else
static void interrupt (*OldIRQ)();
#endif

/****************************************************************************
** Checks to see if a Sound Blaster exists at a given address, returns     **
** true if Sound Blaster found, false if not.                              **
****************************************************************************/
static int ResetDSP(unsigned int Test)
{
  //Reset the DSP
  outp (Test + 0x6, 1);
  delay(10);
  outp (Test + 0x6, 0);
  delay(10);
  //Check if (reset was succesfull
  if (((inp(Test + 0xE) & 0x80) == 0x80) && (inp(Test + 0xA) == 0xAA))
  {
    //DSP was found
    Base = Test;
    return (1);
  }
  else
    //No DSP was found
    return (0);
}

/****************************************************************************
** Send a byte to the DSP (Digital Signal Processor) on the Sound Blaster  **
****************************************************************************/
static void WriteDSP(unsigned char Value)
{
	//Wait for the DSP to be ready to accept data
	// Thanks Geri for fix
	// "If the sb freezes, the system will not freeze"
	int hibaw=0;
	while((unsigned char)(inp(Base+0xC)&0x80))
    {
		hibaw++;
		if(hibaw>65534) break;
    }
	//Send byte
	outp (Base + 0xC, Value);
}

/****************************************************************************
** Starts playback of the buffer. The DMA controller is programmed with    **
** a block length of 32K - the entire buffer. The DSP is instructed to     **
** play blocks of 8K and then generate an interrupt (which allows the      **
** program to clear the parts that have already been played)               **
****************************************************************************/
static void StartPlayBack ()
{
	unsigned int  Page, OffSet;
	long LinearAddress;
  
  WriteDSP (0xD1);                 //DSP-command D1h - Enable speaker, required
                                   //on older SB's
  WriteDSP (0x40);                 //DSP-command 40h - Set sample frequency
  WriteDSP(165);          //Write time constant

  //Convert pointer to linear address
#ifdef DJGPP
  Page = DOSBufOfs >> 16;          //Calculate page
  OffSet = DOSBufOfs & 0xFFFF;     //Calculate offset in the page
#else
  //Convert pointer to linear address
  LinearAddress = FP_SEG (DMABuffer);
  LinearAddress = (LinearAddress << 4) + FP_OFF (DMABuffer);
  Page = LinearAddress >> 16;      //Calculate page
  OffSet = LinearAddress & 0xFFFF; //Calculate offset in the page
#endif
  outp (0x0A, 4 | DMA);        //Mask DMA channel
  outp (0x0C, 0);              //Clear byte pointer
  outp (0x0B, 0x58 | DMA);     //Set mode
  /*
      The mode consists of the following:
      0x58 + x = binary 01 00 10 xx
                        |  |  |  |
                        |  |  |  +- DMA channel
                        |  |  +---- Read operation (the DSP reads from memory)
                        |  +------- Single cycle mode
                        +---------- Block mode
  */

  outp (DMA << 1, OffSet & 0xFF); //Write the offset to the DMA controller
  outp (DMA << 1, OffSet >> 8);

  if (DMA == 0) outp (0x87, Page);
  if (DMA == 1) outp (0x83, Page);
  if (DMA == 3) outp (0x82, Page);

  outp ((DMA << 1) + 1, 0xFF); //Set the block length to 0x7FFF = 32 Kbyte
  outp ((DMA << 1) + 1, 0x7F);

  outp (0x0A, DMA);            //Unmask DMA channel

  WriteDSP (0x48);                 //DSP-command 48h - Set block length
  WriteDSP (0xFF);                 //Set the block length to 0x1FFF = 8 Kbyte
  WriteDSP (0x1F);
  WriteDSP (0x1C);                 //DSP-command 1Ch - Start auto-init playback
}

/****************************************************************************
** Clears an 8K part of the DMA buffer                                     **
****************************************************************************/
static void ClearBuffer (unsigned int Buffer)
{
	char *Address;
#ifdef DJGPP
	//Fill an 8K block in the DMA buffer with 128's - silence
	Address = (char *)(DMABuffer + (Buffer << 13));
	memset (Address, 128, 8192);
	//Copy DMA buffer to DOS memory
	dosmemput(Address, 8192, DOSBufOfs + (Buffer << 13));
#else
	//Fill an 8K block in the DMA buffer with 128's - silence
	Address = (char *)MK_FP (FP_SEG (DMABuffer), FP_OFF (DMABuffer) + (Buffer << 13));
	memset (Address, 128, 8192);
#endif
}

/****************************************************************************
** Mixes a sample with the contents of the DMA buffer                      **
****************************************************************************/
static void MixVoice (struct WaveData *Voice)
{
  unsigned int Counter, beforeOffSet, OffSet, DMAPointer;

  //Read DMA pointer from DMA controller
  DMAPointer = inp (1 + (DMA << 1));
  DMAPointer = DMAPointer + (inp (1 + (DMA << 1)) << 8);

  /*
    DMAPointer contains amount that remains to be played.
    This is convert to the offset of the current sample
  */

  DMAPointer = 0x7FFF - DMAPointer;

#ifdef DJGPP
  beforeOffSet = OffSet = DMAPointer;

  for (Counter = 0; Counter <= Voice->SoundLength; Counter++) {
    //Mix byte
    DMABuffer [OffSet++] += Voice->Sample [Counter];
    OffSet &= 0x7FFF; //Move on to next byte and keep it in the 32K range
  }

  //Copy as one or in parts
  if (OffSet > beforeOffSet)
    dosmemput (DMABuffer + beforeOffSet, OffSet - beforeOffSet, DOSBufOfs + beforeOffSet);
  else {
    dosmemput (DMABuffer + beforeOffSet, 32768 - beforeOffSet, DOSBufOfs + beforeOffSet);
    dosmemput (DMABuffer, OffSet, DOSBufOfs);
  }
#else
  OffSet = DMAPointer;

  for (Counter = 0; Counter <= Voice->SoundLength; Counter++) {
    //Mix byte
    DMABuffer [OffSet++] += Voice->Sample [Counter];
    OffSet &= 0x7FFF; //Move on to next byte and keep it in the 32K range
  }
#endif
}

/****************************************************************************
** Loads a wave file into memory. This procedure treats any file as a      **
** standard 11025Hz, 8bit, mono .WAV file. It doesn't perform any error    **
** checking.                                                               **
****************************************************************************/
static void LoadVoice (struct WaveData *Voice, const char *FileName)
{
	FILE *WAVFile;
	//If it can't be opened...
	WAVFile = fopen(FileName, "rb");
	if (WAVFile == NULL) {
		//..display error message
		printf ("Unable to open wave file\n");
		return;
	}

	//Return length of file for sound length minus 48 bytes for .WAV header
	fseek(WAVFile, 0L, SEEK_END);
	Voice->SoundLength = ftell (WAVFile) - 48;
	fseek(WAVFile, 0L, SEEK_SET);

	Voice->Sample = (unsigned char *)malloc(Voice->SoundLength + 2); //Assign memory
	fseek(WAVFile, 46L, SEEK_SET);                      //Skip the header

	//Load the sample data
	fread(Voice->Sample, Voice->SoundLength + 2, 1, WAVFile);

	fclose(WAVFile); //Close the file
}

/****************************************************************************
** Converts a wave file so it can be mixed easily                          **
****************************************************************************/
static void ConvertVoice (struct WaveData *Voice)
{
 unsigned int OffSet;

 //for each sample, decrease sample value to avoid overflow

 for (OffSet = 0; OffSet <= Voice->SoundLength; OffSet++) {
   Voice->Sample [OffSet] >>= 2;
   Voice->Sample [OffSet] -= 32;
 }
}

/****************************************************************************
** IRQ service routine - this is called when the DSP has finished playing  **
** a block                                                                 **
****************************************************************************/
#ifdef DJGPP
static void ServiceIRQ ()
#else
static void interrupt ServiceIRQ ()
#endif
{
#ifdef DJGPP
  //Relieve DSP
  inp (0x22E);
#else
  //Relieve DSP
  inp (Base + 0xE);
#endif
  //Acknowledge hardware interrupt
  outp (0x20, 0x20);
  //Acknowledge cascade interrupt for IRQ 2, 10 and 11
  if (IRQ == 2 || IRQ == 10 || IRQ == 11) outp (0xA0, 0x20);
  //Increase pointer to clear buffer and keep it in the range 0..3
  CBuffer++;
  CBuffer &= 3;
  //Clear buffer
  ClearBuffer (CBuffer);
}

/****************************************************************************
** This procedure allocates 32K of memory to the DMA buffer and makes sure **
** that no page boundary is crossed                                        **
****************************************************************************/
static void AssignBuffer ()
{
#ifdef DJGPP
  _go32_dpmi_seginfo TempBuf; //Temporary pointer
  unsigned int  Page1, Page2; //Words

  //Assign 32K to DMA Buffer
  DMABuffer = (unsigned char *)malloc (32768);

  //Assign 32K (2048 paragraphs) of DOS memory
  TempBuf.size = 2048;
  _go32_dpmi_allocate_dos_memory (&TempBuf);

  //Calculate linear address
  DOSBufOfs = TempBuf.rm_segment << 4;

  //Calculate page at start of buffer
  Page1 = DOSBufOfs >> 16;

  //Calculate page at end of buffer}
  Page2 = (DOSBufOfs + 32767) >> 16;

  //Check to see if a page boundary is crossed
  if (Page1 != Page2) {
    //If so, assign another part of memory to the buffer
    DOSBuf.size = 2048;
    _go32_dpmi_allocate_dos_memory (&DOSBuf);
    DOSBufOfs = DOSBuf.rm_segment << 4;
    _go32_dpmi_free_dos_memory (&TempBuf);
  } else //otherwise, use the part we've already allocated
    DOSBuf = TempBuf;

  //Clear DMA buffers
  memset (DMABuffer, 128, 0x7FFF);
  dosmemput (DMABuffer, 32768, DOSBufOfs);
#else
  char *TempBuf; //Temporary pointer
  long LinearAddress;
  unsigned int Page1, Page2;  //Words

  //Assign 32K of memory
  TempBuf = (char *)malloc(32768);

  //Calculate linear address
  LinearAddress = FP_SEG (TempBuf);
  LinearAddress = (LinearAddress << 4) + FP_OFF (TempBuf);

  //Calculate page at start of buffer
  Page1 = LinearAddress >> 16;

  //Calculate page at end of buffer}
  Page2 = (LinearAddress + 32767) >> 16;

  //Check to see if a page boundary is crossed
  if (Page1 != Page2) {
    //If so, assign another part of memory to the buffer
    DMABuffer = (char *)malloc(32768);
    free (TempBuf);
  } else //otherwise, use the part we've already allocated
    DMABuffer = TempBuf;

  memset (DMABuffer, 128, 0x7FFF);
#endif
}

#ifdef DJGPP
//Interrupt vector helpers
static void setvect (int Vector)
{
  //Get location of the new keyboard handler
  MyIRQ.pm_offset = (int)ServiceIRQ;
  MyIRQ.pm_selector = _go32_my_cs ();
  //Save the old interrupt handler
  _go32_dpmi_get_protected_mode_interrupt_vector (Vector, &OldIRQ);
  //Set the new interrupt handler in the chain
  _go32_dpmi_chain_protected_mode_interrupt_vector (Vector, &MyIRQ);
}

static void resetvect (int Vector)
{
  //Set interrupt vector to the BIOS handler
  _go32_dpmi_set_protected_mode_interrupt_vector (Vector, &OldIRQ);
}
#endif

static void SBDMA_Init()
{
	unsigned char Temp;

  //Check for Sound Blaster, address: ports 220, 230, 240, 250, 260 or 280
	for (Temp = 1; Temp < 9; Temp++)
	{
		if (Temp != 7)
		if (ResetDSP (0x200 + (Temp << 4))) {
			break;
		}
	}
	if (Temp == 9) {
    //or none at all
		//printf ("No Sound Blaster found\n");
		return;
	}
	//else printf ("Sound Blaster found at %Xh\n", Base);
  
	DMA = 1;
	IRQ = 7;

	//Assign memory to the DMA Buffer
	AssignBuffer ();

	//Save old/set new IRQ vector
	#ifdef DJGPP
	if (IRQ == 2 || IRQ == 10 || IRQ == 11) {
		if (IRQ == 2) setvect (0x71);
		if (IRQ == 10) setvect (0x72);
		if (IRQ == 11) setvect (0x73);
	} else setvect (8 + IRQ);
	#else
	if (IRQ == 2 || IRQ == 10 || IRQ == 11) {
		if (IRQ == 2) _dos_setvect (0x71,  ServiceIRQ);
		if (IRQ == 10) _dos_setvect (0x72,  ServiceIRQ);
		if (IRQ == 11) _dos_setvect (0x73,  ServiceIRQ);
	} else _dos_setvect (8 + IRQ,  ServiceIRQ);
	#endif

	//Enable IRQ
	if (IRQ == 2 || IRQ == 10 || IRQ == 11) {
		if (IRQ == 2) outp (0xA1, inp (0xA1) & 253);
		if (IRQ == 10) outp (0xA1, inp (0xA1) & 251);
		if (IRQ == 11) outp (0xA1, inp (0xA1) & 247);
		outp (0x21, inp (0x21) & 251);
	} else outp (0x21, inp (0x21) & !(1 << IRQ));

	//Set clear buffer to last buffer
	CBuffer = 3;

	//Start playback
	StartPlayBack ();
}

static void SBDMA_Load(const char* WaveFile, struct WaveData *vc)
{
	LoadVoice (vc, WaveFile);
	ConvertVoice (vc);
}

static void SBDMA_Play(struct WaveData *vc)
{
	MixVoice (vc);
}

static void SBDMA_Unload(struct WaveData *vc)
{
	if (vc->Sample)
	{
		free(vc->Sample);
	}
}

static void SBDMA_Close()
{
	//Stops DMA-transfer
	WriteDSP (0xD0);
	WriteDSP (0xDA);

	//Free the memory allocated to the sound buffer
	if (DMABuffer)
	{
		free (DMABuffer);
		DMABuffer = NULL;
	}

	//Free interrupt vectors used to service IRQs
	#ifdef DJGPP
	if (IRQ == 2 || IRQ == 10 || IRQ == 11) {
		if (IRQ == 2) resetvect (0x71);
		if (IRQ == 10) resetvect (0x72);
		if (IRQ == 11) resetvect (0x73);
	} else resetvect (8 + IRQ);
	#else
	if (IRQ == 2 || IRQ == 10 || IRQ == 11) {
		if (IRQ == 2) _dos_setvect (0x71, OldIRQ);
		if (IRQ == 10) _dos_setvect (0x72, OldIRQ);
		if (IRQ == 11) _dos_setvect (0x73, OldIRQ);
	} else _dos_setvect (8 + IRQ, OldIRQ);
	#endif

	//Mask IRQs
	if (IRQ == 2 || IRQ == 10 || IRQ == 11) {
		if (IRQ == 2) outp (0xA1, inp (0xA1) | 2);
		if (IRQ == 10) outp (0xA1, inp (0xA1) | 4);
		if (IRQ == 11) outp (0xA1, inp (0xA1) | 8);
		outp (0x21, inp (0x21) | 4);
	} else outp (0x21, inp (0x21) | (1 << IRQ));
}


SoundDevice SBDMA_device = {
	"SBDMA driver",
	SBDMA_Init,
	SBDMA_Load,
	SBDMA_Play,
	SBDMA_Unload,
	SBDMA_Close
};
