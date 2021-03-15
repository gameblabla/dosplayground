/*-----------------------------

 [util.h]
 - Alexander Brandt 2018
-----------------------------*/

#ifndef KO_UTIL_H
#define KO_UTIL_H

	#include "kobe.h"

	enum ko_print_to
	{
		KO_COUT = 2,
		KO_LOG = 4
	};

	enum ko_access_mode
	{
		KO_READ = 0x00,
		KO_WRITE = 0x01,
		KO_UPDATE = 0x02
	};

	enum ko_writing_mode
	{
		KO_TRUNCATE = 0x01,
		KO_APPEND = 0x02
	};

	enum ko_seek_mode
	{
		KO_START = 0x00,
		KO_ACTUAL = 0x01,
		KO_END = 0x02
	};

	struct ko_file;

	void koPrint(enum ko_print_to, const char* fmt, ...);

	void* koAlloc(uint16_t size);
	void KO_FAR* koFarAlloc(uint16_t size);
	void KO_HUGE* koHugeAlloc(uint32_t size);

	void koFree(void* pointer);
	void koFarFree(void KO_FAR* pointer);
	void koHugeFree(void KO_HUGE* pointer);

	struct ko_file* koOpenFile(const char* filename, enum ko_access_mode, enum ko_writing_mode);
	void koCloseFile(struct ko_file* file);

	uint16_t koReadFile(void* destination, uint16_t size, struct ko_file* file);
	uint16_t koReadFileFar(void KO_FAR* destination, uint16_t size, struct ko_file* file);
	uint16_t koSeekFile(enum ko_seek_mode, uint16_t offset, struct ko_file* file);

#endif
