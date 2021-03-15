/*
K1n9_Duk3's IMF crusher v1.2 - Removes dummy commands from IMF files.
Copyright (C) 2013-2016 K1n9_Duk3

Based on K1n9_Duk3's MIDI to IMF converter (mid2imf.exe)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned short UINT16;
typedef unsigned int UINT32;
typedef unsigned char UINT8;

#define errno_t int

static UINT8 reg_valid[256] =
{
	0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	//0x20
	1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
	1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	//0x40
	1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
	1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	//0x60
	1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
	1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	//0x80
	1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
	1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	//0xA0
	1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
	//0xB0
	1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0,
	//0xC0
	1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	//0xE0
	1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
	1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static UINT8 regs[256], reg_defined[256];

UINT32 IMF_RemoveFirstVoice(UINT8 *data, UINT32 *size)
{
	UINT32 newsize = 0, pos=0;
	UINT32 oldsize = *size;
	UINT8 reg;
	UINT16 pause;
	;
	while(pos < *size)
	{
		reg = data[pos++];
		switch (reg)
		{
			case 0x20:	// mChar
			case 0x23:	// cChar
			case 0x40:	// mScale
			case 0x43:	// cScale
			case 0x60:	// mAttack
			case 0x63:	// cAttack
			case 0x80:	// mSus
			case 0x83:	// cSus
			case 0xA0:	// F-Number (lo)
			case 0xB0:	// F-Number (hi), Octave number & KeyOn bit
			case 0xC0:	// nConn
			case 0xE0:	// mWave
			case 0xE3:	// cWave
				/* 
				 * All these registers affect voice 0 which is reserved for
				 * AdLib sound effects in most games that use IMF music.
				 */

				// skip copying register & value
				pos++;

				if(newsize > 0 && (data[pos] || data[pos+1]))
				{
					// add current pause to last pause in IMF data
					pause = data[newsize-2] + data[pos];
					data[newsize-2] = (UINT8)pause;
					pause >>= 8;
					pause += data[newsize-1] + data[pos+1];
					data[newsize-1] = (UINT8)pause;
				}
				pos += 2;
				break;

			default:
				data[newsize++] = reg;
				data[newsize++] = data[pos++];
				data[newsize++] = data[pos++];
				data[newsize++] = data[pos++];
				break;
		}
	}
	*size = newsize;
	return oldsize-newsize;
}

UINT32 IMF_RemoveDummyCommands(UINT8 *data, UINT32 *size)
{
	UINT32 newsize = 0, pos=0, i;
	UINT32 oldsize = *size;

	memset(regs, 0, 256);
	memset(reg_defined, 0, 256);
	while(pos < *size)
	{
		UINT16 newpause;
		UINT8 reg = data[pos++];
		if (((reg >= 0xB0 && reg <= 0xB8) || reg == 0xBD) && reg_defined[reg] && newsize > 0)
		{
			// don't remove NoteOn/Off commands:
			data[newsize++] = reg;
			data[newsize++] = regs[reg];
			data[newsize++] = 0;
			data[newsize++] = 0;
		}
		reg_defined[reg] = 1;
		regs[reg] = data[pos++];
		newpause = data[pos] | (data[pos+1] << 8);
		pos += 2;
		if(newpause || (pos == *size))
		{
			// write data for all registers:
			for(i=0; i<256; i++)
			{
				if(!reg_defined[i])
					continue;

				data[newsize++] = i;
				data[newsize++] = regs[i];
				data[newsize++] = 0;
				data[newsize++] = 0;

				reg_defined[i] = 0;
			}
			data[newsize-2] = newpause & 0xFF;
			data[newsize-1] = (newpause >> 8) & 0xFF;
		}
	}
	*size = newsize;
	return oldsize-newsize;
}

UINT32 IMF_RemoveRedundantCommands(UINT8 *data, UINT32 *size)
{
	UINT16 newpause, lastpause=0;
	UINT32 newsize = 0, pos=0;
	UINT32 oldsize = *size;

	memset(regs, 0, 256);
	memset(reg_defined, 0, 256);

	while (pos < *size)
	{
		newpause = data[pos+2] | (data[pos+3] << 8);
		if(reg_defined[data[pos]] && (regs[data[pos]] == data[pos+1]))
		{
			newpause += lastpause;
			if (newsize)
			{
				data[newsize-2] = newpause & 0xFF;
				data[newsize-1] = (newpause >> 8) & 0xFF;
			}
			pos += 4;
		} else {
			reg_defined[data[pos]] = 1;
			regs[data[pos]] = data[pos+1];
			// copy imf data:
			data[newsize++] = data[pos++];
			data[newsize++] = data[pos++];
			data[newsize++] = data[pos++];
			data[newsize++] = data[pos++];
		}
		lastpause = newpause;
	}
	*size = newsize;
	return oldsize-newsize;
}

void IMF_ConvertRate(UINT8 *data, UINT32 size, UINT16 fromRate, UINT16 toRate)
{
	double fracPause = 0.0;
	UINT16 pause;

	data++;
	data++;
	while (size)
	{
		pause = data[0] | (data[1] << 8);
		fracPause += (double)(pause*toRate)/fromRate;
		pause = (unsigned short)fracPause;
		fracPause -= pause;
		data[0] = pause & 0xFF;
		data[1] = (pause >> 8) & 0xFF;
		data += 4;
		size -= 4;
	}

}

UINT32 IMF_GetSongLength(UINT8 *data, UINT32 size)
{
	UINT32 i=size/4, samples = 0;

	while(i--)
	{
		data++;
		data++;
		samples += *data++;
		samples += *data++ << 8;
	}
	return samples;
}

void IMF_TruncateSong(UINT8 *data, UINT32 *size, UINT32 startSample, UINT32 stopSample)
{
	UINT32 currentSample=0, pos=0, newPos=0, pause=0;
	UINT8 reg, val;
	
	if(startSample >= stopSample)
	{
		return;
	}

	memset(regs, 0, 256);
	memset(reg_defined, 0, 256);
	
	//printf("Collecting...\n");
	// collect register values for all skipped samples
	while((currentSample < startSample) && (pos < *size))
	{
		while((pause==0) && (pos < *size))
		{
			reg = data[pos++];
			val = data[pos++];
			regs[reg] = val;
			reg_defined[reg] = 1;
			pause = data[pos] | (data[pos+1]) << 8;
			pos += 2;
		}
		pause--;
		currentSample++;
	}
	
	//printf("Writing...\n");
	// write collected register values (if any)
	for(int i=0; i<256; i++)
	{
		if(reg_defined[i])
		{
			// Note: This won't turn off the KeyOn bits, so any note that wasn't 
			// turned off in the block we just skipped/removed will be played at
			// the beginning of the truncated song.
			// This is helpful as it preserves long notes.
			data[newPos++] = i;
			data[newPos++] = regs[i];
			data[newPos++] = 0;
			data[newPos++] = 0;
		}
	}
	if(pause && newPos > 0)
	{
		data[newPos-2] = (UINT8) pause;
		data[newPos-1] = (UINT8) (pause >> 8);
	}
	
	//printf("Copying...\n");
	// copy IMF instructions
	while((currentSample < stopSample) && (pos < *size))
	{
		while((pause==0) && (pos < *size))
		{
			// copy register and value
			data[newPos++] = data[pos++];
			data[newPos++] = data[pos++];

			pause = data[pos] | (data[pos+1]) << 8;
			if(currentSample+pause < stopSample)
			{
				// copy pause
				data[newPos++] = data[pos++];
				data[newPos++] = data[pos++];
				currentSample += pause;
				pause = 0;
			} else {
				// we're at the end of the truncated song

				// write pause
				pause = stopSample-currentSample;
				data[newPos++] = (UINT8) pause;
				data[newPos++] = (UINT8) (pause >> 8);

				// Most IMF players ignore the last pause value,
				// so we must append another instruction to ensure
				// that the pause won't be ignored. We'll simply
				// copy the last instruction to do this.

				// we don't need pos anymore, so we can re-use the variable here
				pos = newPos-4;

				data[newPos++] = data[pos++];
				data[newPos++] = data[pos++];
				data[newPos++] = 0;
				data[newPos++] = 0;

				// update the size value and return
				*size = newPos;
				return;
			}
		}
		pause--;
		currentSample++;
	}

	// this will only be executed if lastSample was beyond the song's total length
	*size = newPos;
}

int IMF_WriteIMF(char* filename, UINT8 *data, UINT32 size, UINT8 chunk)
{
	FILE *out;
	out = fopen(filename, "wb");
	if (out)
	{
		if(chunk)
		{
			UINT8 buf[2];
			buf[0] = size & 0xFF;
			buf[1] = (size >> 8) & 0xFF;
			fwrite(buf, 1, 2, out);
		}
		fwrite(data, 1, size, out);
		fclose(out);
		return 0;
	} else {
		printf("ERROR: could not create %s\n", filename);
		return -1;
	}
}

UINT16 ReadInt16LE(FILE* in)
{
	UINT8 buf[2];
	fread(buf, 1, 2, in);
	return (buf[0] | (buf[1] << 8));
}

int IMF_IsChunk(FILE* in)
{
	UINT16 chunksize, buff, i=42;
	UINT32 sum1=0, sum2=0;

	if(!in)
		return 0;

	if (!feof(in))
	{
		chunksize = ReadInt16LE(in);
		if ((chunksize == 0) || (chunksize & 3))
			return 0;

		while(!feof(in) && i)
		{
			fread(&buff, sizeof(buff), 1, in);
			sum1 += buff;
			fread(&buff, sizeof(buff), 1, in);
			sum2 += buff;
			i--;
		}
		fseek(in, 0, SEEK_SET);
		return (sum1 > sum2);
	}
	return 0;
}

static UINT8 *imfdata = NULL;
static UINT32 imfsize = 0;

void IMF_FixDro(void)
{
	UINT8 *newdata;
	UINT32 newsize=0, i, reg;

	newdata = (UINT8 *)malloc(imfsize+1024);
	if (!newdata)
	{
		printf("ERROR: out of memory in FixDro\n");
		return;
	}

	memset(regs, 0, 256);
	memcpy(reg_defined, reg_valid, 256);

	for (i=0; i<imfsize; i+= 4)
	{
		if (imfdata[i+2] || imfdata[i+3])
			break;
		regs[imfdata[i]] = imfdata[i+1];
		reg_defined[imfdata[i]] = 1;
	}

	for (reg=0; reg<256; reg++)
	{
		if (reg_defined[reg])
		{
			newdata[newsize++] = reg;
			newdata[newsize++] = regs[reg];
			newdata[newsize++] = 0;
			newdata[newsize++] = 0;
		}
	}

	while (i < imfsize)
	{
		newdata[newsize++] = imfdata[i++];
	}

	free(imfdata);
	imfdata = newdata;
	imfsize = newsize;
}

int IMF_LoadIMF(char* filename)
{
	UINT32 size;
	FILE *in;
	in = fopen(filename, "rb");
	if (in)
	{
		if (IMF_IsChunk(in))
		{
			size = ReadInt16LE(in);
		} else {
			fseek(in, 0, SEEK_END);
			size = ftell(in);
			size -= size & 3;	// only multiples of 4 bytes allowed!
			fseek(in, 0, SEEK_SET);
		}
		if(imfdata)
			free(imfdata);
		imfdata = (UINT8*) malloc(size);
		if(imfdata)
		{
			fread(imfdata, 1, size, in);
			imfsize = size;
		} else {
			printf("ERROR: out of memory while reading %s\n", filename);
			fclose(in);
			return -1;
		}
		fclose(in);
		return 0;
	} else {
		printf("ERROR: could not open %s\n", filename);
		return -1;
	}
}

int main(int argc, char* argv[])
{
	printf("K1n9_Duk3's IMF crusher v1.2\n");
	printf("============================\n\n");

	if(argc < 3)
	{
		printf("Usage: IMFCRUSH <infile> <outfile> [OPTIONS]\n\n");
		printf("Recognized options are:\n"
		       " /keepfirst     keep instructions that use the first voice/channel\n"
		       "                (the first voice is usually reserved for AdLib sound effects)\n"
		       " /keepdummy     keep instructions that get overwritten in the same cycle\n"
		       " /keepredundant keep instructions that write the same value that's already in\n"
		       "                the AdLib register\n"
		       " /keepall       combination of /keepfirst /keepdummy and /keepredundant\n"
		       " /chunk         saves an AUDIOT chunk instead of a plain IMF file\n"
		       " \n"
		       " /convert <fromRate> <toRate>\n"
		       "                converts the song to a different rate\n"
		       " /truncate <startSample> <stopSample>\n"
		       "                keeps only samples from <startSample> until <stopSample>\n"
				 " /fixdro        fixes issues in IMF files created by DRO2IMF\n"
		       " \n"
			   );

	} else {
		UINT32 truncate=0, startSample, stopSample;
		int convert=0, fromRate, toRate;
		int keepd=0, keepr=0, keepf=0, chunk=0, fixdro=0;

		#define IFARG(str) if(!strcasecmp(arg, (str)))

		for(int i = 3; i < argc; i++)
		{
			char* arg = argv[i];
			IFARG("/keepdummy")
				keepd=1;
			else IFARG("/keepredundant")
				keepr=1;
			else IFARG("/keepfirst")
				keepf=1;
			else IFARG("/keepall")
				keepd=keepr=keepf=1;
			else IFARG("/chunk")
				chunk = 1;
			else IFARG("/fixdro")
				fixdro = 1;
			else IFARG("/nochunk")
				chunk = 0;
			else IFARG("/convert")
				if (i+2 >= argc)
				{
					printf("ERROR: /convert needs 2 arguments\n");
					return 1;
				} else {
					convert = 1;
					fromRate = atoi(argv[++i]);
					if (fromRate <= 0)
					{
						printf("ERROR: <fromRate> must be greater than 0\n");
						return 1;
					}
					toRate = atoi(argv[++i]);
					if (toRate <= 0)
					{
						printf("ERROR: <toRate> must be greater than 0\n");
						return 1;
					}
				}
			else IFARG("/truncate")
				if (i+2 >= argc)
				{
					printf("ERROR: /truncate needs 2 arguments\n");
					return 1;
				} else {
					truncate = 1;
					startSample = atoi(argv[++i]);
					stopSample = atoi(argv[++i]);
					if (startSample >= stopSample)
					{
						printf("ERROR: <stopSample> must be larger than <startSample>\n");
						return 1;
					}
				}
			else
			{
				printf("ERROR: unhandled option: %s\n", arg);
				return 1;
			}
		}

		printf("Loading IMF song from %s\n", argv[1]);
		if (IMF_LoadIMF(argv[1]) == 0)
		{
			if (fixdro)
			{
				UINT32 oldsize = imfsize;
				IMF_FixDro();
				printf("FixDro changed song size from %u bytes to %u bytes).\n", oldsize, imfsize);
			}
			if (truncate)
			{
				UINT32 samples = IMF_GetSongLength(imfdata, imfsize);
				if(startSample >= samples)
				{
					printf("ERROR: <startSample> is out of range (song length is only %u samples)\n", samples);
					return 1;
				}
				if(stopSample >= samples)
				{
					printf("ERROR: <stopSample> is out of range (song length is only %u samples)\n", samples);
					return 1;
				}
				printf("Truncating IMF, keeping samples from %u until %u \n", startSample, stopSample);
				IMF_TruncateSong(imfdata, &imfsize, startSample, stopSample);
			}
			if (convert)
			{
				printf("Converting IMF from %d Hz to %d Hz\n", fromRate, toRate);
				IMF_ConvertRate(imfdata, imfsize, fromRate, toRate);
			}
			if (!keepf)
			{
				UINT32 tmp = IMF_RemoveFirstVoice(imfdata, &imfsize);
				printf("RemoveFirstVoice removed %u bytes (%u commands).\n", tmp, tmp/4);
			}
			if (!keepd)
			{
				UINT32 tmp = IMF_RemoveDummyCommands(imfdata, &imfsize);
				printf("RemoveDummyCommands removed %u bytes (%u commands).\n", tmp, tmp/4);
			}
			if(!keepr)
			{
				UINT32 tmp = IMF_RemoveRedundantCommands(imfdata, &imfsize);
				printf("RemoveRedundantCommands removed %u bytes (%u commands).\n", tmp, tmp/4);
			}

			printf("Writing IMF song to %s", argv[2]);
			if(chunk)
				printf(" (as AUDIOT chunk)\n");
			else
				printf(" (as plain IMF data)\n");

			IMF_WriteIMF(argv[2], imfdata, imfsize, chunk);
		}
	}
}
