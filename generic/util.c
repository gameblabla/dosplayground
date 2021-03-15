/*-----------------------------

MIT License

Copyright (c) 2018 Alexander Brandt

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-------------------------------

 [util.c]
 - Alexander Brandt 2018

 TODO: rather that a log file, use
       some modem hardware/serial
-----------------------------*/

#include "util.h"


struct ko_file
{
	uint16_t fd;
};


static int s_mallocs_freeded_no = 0;
static int s_mallocs_no = 0;

static bool s_set_close;

static void s_close()
{
	//koPrint(KO_LOG, "\ntotal memory allocs: %i \n", s_mallocs_no);
	//koPrint(KO_LOG, "freeded memory allocs: %i \n", s_mallocs_freeded_no);

	if ((s_mallocs_no - s_mallocs_freeded_no) != 0)
		//koPrint(KO_LOG, "[error] | memory leak(s)\n");
}

/*-----------------------------

 koAlloc()
-----------------------------*/
void* koAlloc(uint16_t size)
{
	void* mem = NULL;

	if (s_set_close == false)
	{
		koDosHeapGrow(); /* TODO: documentation */

		s_set_close = true;
		atexit(s_close);
	}

	if ((mem = malloc(size)) != NULL)
		s_mallocs_no++;

	return mem;
}

void KO_FAR* koFarAlloc(uint16_t size)
{
	void KO_FAR* mem = NULL;

	if (s_set_close == false)
	{
		koDosHeapGrow(); /* TODO: documentation */

		s_set_close = true;
		atexit(s_close);
	}

	if ((mem = koDosFarMemoryAlloc(size)) != NULL)
		s_mallocs_no++;

	return mem;
}

void KO_HUGE* koHugeAlloc(uint32_t size)
{
	void KO_HUGE* mem = NULL;

	if (s_set_close == false)
	{
		koDosHeapGrow(); /* TODO: documentation */

		s_set_close = true;
		atexit(s_close);
	}

	if ((mem = koDosHugeMemoryAlloc(size, 1)) != NULL)
		s_mallocs_no++;

	return mem;
}


/*-----------------------------

 koFree()
-----------------------------*/
void koFree(void* p)
{
	s_mallocs_freeded_no++;
	free(p);
}

void koFarFree(void KO_FAR* p)
{
	s_mallocs_freeded_no++;
	koDosFarMemoryFree(p);
}

void koHugeFree(void KO_HUGE* p)
{
	s_mallocs_freeded_no++;
	koDosHugeMemoryFree(p);
}


/*-----------------------------

 koOpenFile()
-----------------------------*/
struct ko_file* koOpenFile(const char* filename, enum ko_access_mode access_mode, enum ko_writing_mode write_mode)
{
	/*
	 Dos, Open Existing File (Int 21/AH=3Dh)
	 - http://www.ctyme.com/intr/rb-2779.htm
	 - http://stanislavs.org/helppc/int_21-3d.html
	*/

	union ko_regs reg;
	struct ko_status_regs sreg;

	struct ko_file* file = NULL;

	if ((file = malloc(sizeof(struct ko_file))) == NULL)
	{
		//koPrint(KO_LOG, "[Error] Out of memory\n");
		goto return_failure;
	}

	reg.h.ah = 0x3D;
	reg.h.al = access_mode;

	sreg.ds = koDosPointerSegment(filename);
	reg.w.dx = koDosPointerOffset(filename);

	koDosInterruptX(0x21, &reg, &reg, &sreg);

	if (reg.w.cflag == 1)
	{
		//koPrint(KO_LOG, "[Error] Int: 21/AH=3Dh, code: 0x%02X\n", reg.w.ax);
		goto return_failure;
	}
	else
		file->fd = reg.w.ax;

	/* Bye! */
	//koPrint(KO_LOG, "File '%s' succesfully opened, fd: %u\n", filename, file->fd);
	return file;

return_failure:
	//koPrint(KO_LOG, "[Error] koOpenFile(), file: '%s'\n", filename);

	if (file != NULL)
		free(file);

	return NULL;
}


/*-----------------------------

 koCloseFile()
-----------------------------*/
void koCloseFile(struct ko_file* file)
{
	/*
	 Dos, Close File (Int 21/AH=3Eh)
	 - http://www.ctyme.com/intr/rb-2782.htm
	*/

	union ko_regs reg;

	reg.h.ah = 0x3E;
	reg.w.bx = file->fd;
	koDosInterrupt(0x21, &reg, &reg);

	if (reg.w.cflag != 0)
		//koPrint(KO_LOG, "[Error] Closing file fd: %u\n", file->fd);

	free(file);
}


/*-----------------------------

 koReadFile()
-----------------------------*/
uint16_t koReadFile(void* dest, uint16_t size, struct ko_file* fp)
{
	union ko_regs reg;
	struct ko_status_regs sreg;

	/*
	 Dos, Read From File or Device (Int 21/AH=3Fh)
	 - http://www.ctyme.com/intr/rb-2783.htm
	*/
	reg.h.ah = 0x3F;

	reg.w.bx = fp->fd;
	reg.w.cx = size;
	sreg.ds = koDosPointerSegment(dest);
	reg.w.dx = koDosPointerOffset(dest);

	koDosInterruptX(0x21, &reg, &reg, &sreg);

	if (reg.w.cflag != 0)
	{
		//koPrint(KO_LOG, "[error] | Int 21/AH=3Fh (code 0x%02X) [fd: %u]\n", reg.w.ax, fp->fd);
		return 1; //Error :(
	}

	// return reg.w.ax; /* bytes read */
	return 0; // Success!
}

uint16_t koReadFileFar(void KO_FAR* dest, uint16_t size, struct ko_file* fp)
{
	/* The same as above, except for the 'KO_FAR' attribute */
	union ko_regs reg;
	struct ko_status_regs sreg;

	reg.h.ah = 0x3F;

	reg.w.bx = fp->fd;
	reg.w.cx = size;
	sreg.ds = koDosPointerSegment(dest);
	reg.w.dx = koDosPointerOffset(dest);

	koDosInterruptX(0x21, &reg, &reg, &sreg);

	if (reg.w.cflag != 0)
	{
		//koPrint(KO_LOG, "[error] | Int 21/AH=3Fh (code 0x%02X) [fd: %u]\n", reg.w.ax, fp->fd);
		return 1; // error :8
	}

	// return reg.w.ax;
	return 0; // Success!
}


/*-----------------------------

 koSeekFile()
-----------------------------*/
uint16_t koSeekFile(enum ko_seek_mode mode, uint16_t offset, struct ko_file* fp)
{
	union ko_regs reg;

	/*
	 Dos, Set Current File Position (Int 21/AH=42h)
	 - http://www.ctyme.com/intr/rb-2799.htm

	 FIXME: "For origins 01h and 02h, the pointer may be positioned before
	 the start of the file; no error is returned in that case"
	*/

	reg.h.ah = 0x42;

	reg.h.al = mode;
	reg.w.bx = fp->fd;

	reg.w.cx = 0x0000;
	reg.w.dx = offset;

	koDosInterrupt(0x21, &reg, &reg);

	if (reg.w.cflag != 0)
		return 1;
		// return offset - 1;

	// return offset;
	return 0; // Success!
}
