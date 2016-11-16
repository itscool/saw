// saw_io.h - Cross platform file system manipulation
//          - Io abstraction including file and memory implementations
//          - Bit streaming
//          - Bit twiddling and byte swapping          
//
// This is free and unencumbered software released into the public domain.
// 
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
//
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// For more information, please refer to <http://unlicense.org/>

//-----------------------------------------------------------------------------------------------------------
// History
// - v1.08 - 11/16/16 - Fixed IoReadLine8 and IoReadText8 for 0 length strings and return value
// - v1.07 - 06/20/16 - Fix for async delete-then-recreate problem in Windows
// - v1.06 - 04/28/16 - Committed to C++ style strings and expectations
//                    - Bit/Byte functions more consistent
//                    - Reduced dependencies
// - v1.05 - 04/20/16 - Fixed FsWatch on 32-bit platforms
//                    - Fixed FsWalk in linux
// - v1.04 - 04/16/16 - Added FsGetRelPath
//                    - Added FsWatch
//                    - Added FsWalk
// - v1.03 - 03/30/16 - Fixed a slightly ambiguous placement of braces
// - v1.02 - 03/17/16 - Merged with bit streaming, bit twiddling, byte swapping libraries
//                    - Merged with file system manipulation library
// - v1.01 - 03/17/16 - Fixed treat C-string like std::string in linux IoOpenFile
// - v1.00 - 03/12/16 - Initial release by Scott Williams

//-----------------------------------------------------------------------------------------------------------
// Notes
// - Simple, tested C++ solution for most filesystem i/o needs

//-----------------------------------------------------------------------------------------------------------
// Usage
// - Define SAW_IO_IMPLEMENTATION before inclusion in one file for library implementation
// - Define SAW_IO_NO_VECTOR to remove std::vector dependencies
// - Functionality is in saw:: namespace

//-----------------------------------------------------------------------------------------------------------
// Todo

#ifndef _SAW_IO_H_INCLUDED
#define _SAW_IO_H_INCLUDED

#include <string>
#ifndef SAW_IO_NO_VECTOR
#	include <vector>
#endif

namespace saw {

// Returning false stops the file tree walk
typedef bool(*FsWalkFunc)(const std::string &fullPath, bool isFile, void *user);

enum FsWatchType {
	FS_WATCH_NONE = 0,
	FS_WATCH_REMOVED,
	FS_WATCH_CHANGED,
	FS_WATCH_CREATED,
};

enum FsWalkType {
	FS_WALK_NORMAL,    // top-down traversal of file tree
	FS_WALK_DEPTH,     // depth-first traversal of file tree
};

enum IoSeekType {
	IO_SEEK_SET,
	IO_SEEK_REL,
	IO_SEEK_END
};

enum IoAccessType {
	IO_ACCESS_RW_NEW,
	IO_ACCESS_RW,
	IO_ACCESS_R
};

struct Io {
	void *handle;
	bool (*funcRead)(void *, size_t, void *);
	bool (*funcWrite)(void *, size_t, const void *);
	bool (*funcSeek)(void *, IoSeekType, long long);
	long long (*funcTell)(void *);
	void (*funcClose)(void *);
};

struct BitStreamIn {
	const unsigned char *pBase;
	const unsigned char *pPos;
	unsigned int hasBits;
	unsigned int currBits;
	unsigned int bytesLeft;
};

struct BitStreamOut {
	unsigned char *pBase;
	unsigned char *pPos;
	unsigned int hasBits;
	unsigned int currBits;
	unsigned int bytesLeft;
};

// Cross platform file system manipulation
bool FsCreateDir(const std::string &fullPathDir);
bool FsDeleteDir(const std::string &fullPathDir);
bool FsDeleteFile(const std::string &fullPathFile);
bool FsRename(const std::string &fullPathOld, const std::string &fullPathNew);
bool FsSetWorkingDir(const std::string &fullPathDir);
bool FsIsDir(const std::string &fullPath);
bool FsIsFile(const std::string &fullPath);
bool FsIsReadOnly(const std::string &fullPath);
bool FsIsHidden(const std::string &fullPath);
bool FsIsRelative(const std::string &path);
time_t FsGetTimeAccess(const std::string &fullPath);
time_t FsGetTimeMod(const std::string &fullPath);
time_t FsGetTimeCreate(const std::string &fullPath);
std::string FsGetTempDir(bool includeSlash);
std::string FsGetWorkingDir(bool includeSlash);
std::string FsGetAppDir(bool includeSlash);
std::string FsGetAppPath();
std::string FsGetRelPath(const std::string &fullPathFrom, const std::string &fullPathTo);
std::string FsGetFullPath(const std::string &path);
std::string FsGetPathDir(const std::string &fullPath, bool includeSlash);
std::string FsGetPathFile(const std::string &fullPath, bool includeExt);
std::string FsGetPathExt(const std::string &fullPath, bool includeDot);
unsigned long long FsGetFileSize(const std::string &fullPathFile);
FsWatchType FsWatch(const std::string &fullPath, int *state); // init state to 0
bool FsWalk(const std::string &fullPath, FsWalkType type, FsWalkFunc callback, void *user);

// Io abstraction - implementations
bool IoOpenFile(Io *io, const std::string &filename, IoAccessType access);
bool IoOpenMem(Io *io, const void *pMem, size_t size);
#ifndef SAW_IO_NO_VECTOR
bool IoOpenVec(Io *io, std::vector<char> *pVec);
#endif
void IoClose(Io *io);

// Io abstraction - general
inline bool IoSeek(const Io *io, IoSeekType type, long long offset);
inline bool IoSkip(const Io *io, long long offset);
inline long long IoSize(const Io *io);
inline long long IoTell(const Io *io);
inline int IoRead8(const Io *io);
inline int IoRead16le(const Io *io);
inline int IoRead16be(const Io *io);
inline int IoRead32le(const Io *io);
inline int IoRead32be(const Io *io);
inline float IoReadFle(const Io *io);
inline float IoReadFbe(const Io *io);
inline double IoReadDle(const Io *io);
inline double IoReadDbe(const Io *io);
inline bool IoReadRaw(const Io *io, size_t bytes, void *pOut);
bool IoReadText8(const Io *io, char *buf, int bufSize);  // Stops at null, eof, or bufSize; bufSize must consider null terminator
bool IoReadLine8(const Io *io, char *buf, int bufSize);  // Stops at null, eof, eol, or bufSize; bufSize must consider null terminator; does NOT include eol character(s) in output
bool IoReadText8(const Io *io, std::string *pOut, int len);  // Stops at null, eof, or len if >= 0 for more secure reads
bool IoReadLine8(const Io *io, std::string *pOut);  // Stops at null, eof, or eol
inline void IoWrite8(const Io *io, int val);
inline void IoWrite16le(const Io *io, int val);
inline void IoWrite16be(const Io *io, int val);
inline void IoWrite32le(const Io *io, int val);
inline void IoWrite32be(const Io *io, int val);
inline void IoWriteFle(const Io *io, float val);
inline void IoWriteFbe(const Io *io, float val);
inline void IoWriteDle(const Io *io, double val);
inline void IoWriteDbe(const Io *io, double val);
inline void IoWriteRaw(const Io *io, size_t bytes, const void *pIn);
void IoWriteSle(const Io *io, const char *format, ...);
void IoWriteSbe(const Io *io, const char *format, ...);
void IoWriteText8(const Io *io, const char *in, bool includeNull);
void IoWriteLine8(const Io *io, const char *in);  // ensures newline

// Bit streaming
inline void BsInit(BitStreamIn *pBits, const void *pBase, unsigned int byteSize);
inline void BsInit(BitStreamOut *pBits, void *pBase, unsigned int byteSize);
inline bool BsReadLsb(BitStreamIn *pBits, unsigned int *pOut, unsigned char numBits);
inline bool BsReadMsb(BitStreamIn *pBits, unsigned int *pOut, unsigned char numBits);
inline void BsReadFlush(BitStreamIn *pBits);  // Flush partial byte if there is one
inline bool BsWriteLsb(BitStreamOut *pBits, unsigned int in, unsigned char numBits);
inline bool BsWriteMsb(BitStreamOut *pBits, unsigned int in, unsigned char numBits);
inline void BsWriteFlushLsb(BitStreamOut *pBits);  // Flush partial byte if there is one
inline void BsWriteFlushMsb(BitStreamOut *pBits);  // Flush partial byte if there is one
template <class BitStream> inline void BsSetPosLsb(BitStream *pBits, unsigned int bitPos);
template <class BitStream> inline void BsSetPosMsb(BitStream *pBits, unsigned int bitPos);
template <class BitStream> inline void BsSkipLsb(BitStream *pBits, int numBits);
template <class BitStream> inline void BsSkipMsb(BitStream *pBits, int numBits);
template <class BitStream> inline void BsUndoLsb(BitStream *pBits, unsigned int in, unsigned char numBits);
template <class BitStream> inline void BsUndoMsb(BitStream *pBits, unsigned int in, unsigned char numBits);

// Bit twiddling and byte swapping
inline unsigned char BitReverse8(unsigned char value);
inline unsigned short BitReverse16(unsigned short value);
inline unsigned int BitReverse32(unsigned int value);
inline int BitGetCount(unsigned int value);
inline int BitGetMsb(unsigned int value);
inline int BitGetLsb(unsigned int value);
void BitInvertBuf(void *pData, size_t numBytes);
void BitReverseBuf8(void *pData, size_t numBytes);
inline unsigned short ByteSwap16(unsigned short value);
inline unsigned int ByteSwap32(unsigned int value);
inline unsigned long long ByteSwap64(unsigned long long value);
void ByteSwapBuf16(void *pData, size_t count); // count in words, not bytes
void ByteSwapBuf32(void *pData, size_t count); // count in double words, not bytes

//-----------------------------------------------------------------------------------------------------------
inline bool IoSeek(const Io *io, IoSeekType type, long long offset) {
	return io->funcSeek(io->handle, type, offset);
}

//-----------------------------------------------------------------------------------------------------------
inline bool IoSkip(const Io *io, long long offset) {
	return io->funcSeek(io->handle, IO_SEEK_REL, offset);
}

//-----------------------------------------------------------------------------------------------------------
inline long long IoSize(const Io *io) {
	long long pos = io->funcTell(io->handle);
	io->funcSeek(io->handle, IO_SEEK_END, 0);
	long long size = io->funcTell(io->handle);
	io->funcSeek(io->handle, IO_SEEK_SET, pos);
	return size;
}

//-----------------------------------------------------------------------------------------------------------
inline long long IoTell(const Io *io) {
	return io->funcTell(io->handle);
}

//-----------------------------------------------------------------------------------------------------------
inline int IoRead8(const Io *io) {
	unsigned char ret;
	io->funcRead(io->handle, 1, &ret);
	return static_cast<int>(ret);
}

//-----------------------------------------------------------------------------------------------------------
inline int IoRead16be(const Io *io) {
	int high = IoRead8(io) << 8;
	return high | IoRead8(io);
}

//-----------------------------------------------------------------------------------------------------------
inline int IoRead16le(const Io *io) {
	int low = IoRead8(io);
	return low | (IoRead8(io) << 8);
}

//-----------------------------------------------------------------------------------------------------------
inline int IoRead32be(const Io *io) {
	int high = IoRead16be(io) << 16;  // Don't want 16 bit value to be sign-extended
	return high | IoRead16be(io);
}

//-----------------------------------------------------------------------------------------------------------
inline int IoRead32le(const Io *io) {
	int low = IoRead16le(io);  // Don't want 16 bit value to be sign-extended
	return low | (IoRead16le(io) << 16);
}

//-----------------------------------------------------------------------------------------------------------
inline float IoReadFbe(const Io *io) {
	int low = IoRead16be(io) << 16;  // Don't want 16 bit value to be sign-extended
	low |= IoRead16be(io);
	return *reinterpret_cast<float *>(&low);
}

//-----------------------------------------------------------------------------------------------------------
inline float IoReadFle(const Io *io) {
	int low = IoRead16le(io);  // Don't want 16 bit value to be sign-extended
	low |= IoRead16le(io) << 16;
	return *reinterpret_cast<float *>(&low);
}

//-----------------------------------------------------------------------------------------------------------
inline double IoReadDbe(const Io *io) {
	unsigned long long high = IoRead32be(io);
	unsigned long long low = IoRead32be(io);
	low |= high << 32;
	return *reinterpret_cast<double *>(&low);
}

//-----------------------------------------------------------------------------------------------------------
inline double IoReadDle(const Io *io) {
	unsigned long long low = IoRead32le(io);
	unsigned long long high = IoRead32le(io);
	low |= high << 32;
	return *reinterpret_cast<double *>(&low);
}

//-----------------------------------------------------------------------------------------------------------
inline bool IoReadRaw(const Io *io, size_t size, void *pOut) {
	if (size == 0)
		return true;
	return io->funcRead(io->handle, size, pOut);
}

//-----------------------------------------------------------------------------------------------------------
inline void IoWrite8(const Io *io, int val) {
	io->funcWrite(io->handle, 1, &val);
}

//-----------------------------------------------------------------------------------------------------------
inline void IoWrite16be(const Io *io, int val) {
	IoWrite8(io, val >> 8);
	IoWrite8(io, val & 0xff);
}

//-----------------------------------------------------------------------------------------------------------
inline void IoWrite16le(const Io *io, int val) {
	IoWrite8(io, val & 0xff);
	IoWrite8(io, val >> 8);
}

//-----------------------------------------------------------------------------------------------------------
inline void IoWrite32be(const Io *io, int val) {
	IoWrite16be(io, val >> 16);
	IoWrite16be(io, val & 0xffff);
}

//-----------------------------------------------------------------------------------------------------------
inline void IoWrite32le(const Io *io, int val) {
	IoWrite16le(io, val & 0xffff);
	IoWrite16le(io, val >> 16);
}

//-----------------------------------------------------------------------------------------------------------
inline void IoWriteFbe(const Io *io, float val) {
	IoWrite32be(io, *reinterpret_cast<int *>(&val));
}

//-----------------------------------------------------------------------------------------------------------
inline void IoWriteFle(const Io *io, float val) {
	IoWrite32le(io, *reinterpret_cast<int *>(&val));
}

//-----------------------------------------------------------------------------------------------------------
inline void IoWriteDbe(const Io *io, double val) {
	unsigned long long v64 = *reinterpret_cast<unsigned long long *>(&val);
	IoWrite32be(io, v64 >> 32);
	IoWrite32be(io, v64 & 0xffffffff);
}

//-----------------------------------------------------------------------------------------------------------
inline void IoWriteDle(const Io *io, double val) {
	unsigned long long v64 = *reinterpret_cast<unsigned long long *>(&val);
	IoWrite32le(io, v64 & 0xffffffff);
	IoWrite32le(io, v64 >> 32);
}

//-----------------------------------------------------------------------------------------------------------
inline void IoWriteRaw(const Io *io, size_t bytes, const void *pIn) {
	if (bytes == 0)
		return;
	io->funcWrite(io->handle, bytes, pIn);
}

//-------------------------------------------------------------------------------------------------------
inline void BsInit(BitStreamIn *pBits, const void *pBase, unsigned int byteSize) {
	pBits->pPos = pBits->pBase = reinterpret_cast<const unsigned char *>(pBase);
	pBits->bytesLeft = byteSize;
	pBits->hasBits = pBits->currBits = 0;
}

//-----------------------------------------------------------------------------------------------------------
inline void BsInit(BitStreamOut *pBits, void *pBase, unsigned int byteSize) {
	pBits->pPos = pBits->pBase = reinterpret_cast<unsigned char *>(pBase);
	pBits->bytesLeft = byteSize;
	pBits->hasBits = pBits->currBits = 0;
}

//-----------------------------------------------------------------------------------------------------------
inline bool BsReadLsb(BitStreamIn *pBits, unsigned int *pOut, unsigned char numBits) {
	while (pBits->hasBits < numBits) {
		if (pBits->bytesLeft == 0)
			return false;
		pBits->currBits |= *pBits->pPos++ << pBits->hasBits;
		pBits->bytesLeft--;
		pBits->hasBits += 8;
	}
	*pOut = pBits->currBits & ((1 << numBits) - 1);
	pBits->currBits >>= numBits;
	pBits->hasBits -= numBits;
	return true;
}

//-----------------------------------------------------------------------------------------------------------
inline bool BsReadMsb(BitStreamIn *pBits, unsigned int *pOut, unsigned char numBits) {
	while (pBits->hasBits < numBits) {
		if (pBits->bytesLeft == 0)
			return false;
		pBits->currBits |= *pBits->pPos++ << (24 - pBits->hasBits);
		pBits->bytesLeft--;
		pBits->hasBits += 8;
	}
	*pOut = pBits->currBits >> (32 - numBits);
	pBits->currBits <<= numBits;
	pBits->hasBits -= numBits;
	return true;
}

//-----------------------------------------------------------------------------------------------------------
inline void BsReadFlush(BitStreamIn *pBits) {
	if (pBits->hasBits) {
		pBits->pPos++;
		pBits->bytesLeft--;
		pBits->currBits = 0;
		pBits->hasBits = 0;
	}
}

//-----------------------------------------------------------------------------------------------------------
inline bool BsWriteLsb(BitStreamOut *pBits, unsigned int in, unsigned char numBits) {
	pBits->currBits |= in << pBits->hasBits;
	pBits->hasBits += numBits;
	while (pBits->hasBits >= 8) {
		*pBits->pPos++ = pBits->currBits & 0xff;
		pBits->bytesLeft--;
		pBits->currBits >>= 8;
		pBits->hasBits -= 8;
	}
	return true;
}

//-----------------------------------------------------------------------------------------------------------
inline bool BsWriteMsb(BitStreamOut *pBits, unsigned int in, unsigned char numBits) {
	pBits->currBits |= in << (32 - numBits - pBits->hasBits);
	pBits->hasBits += numBits;
	while (pBits->hasBits >= 8) {
		*pBits->pPos++ = pBits->currBits >> 24;
		pBits->bytesLeft--;
		pBits->currBits <<= 8;
		pBits->hasBits -= 8;
	}
	return true;
}

//-----------------------------------------------------------------------------------------------------------
inline void BsWriteFlushLsb(BitStreamOut *pBits) {
	BsWriteLsb(pBits, 0, 0);
	if (pBits->hasBits) {
		unsigned char mask = (1 << pBits->hasBits) - 1;
		*pBits->pPos = *pBits->pPos & ~mask;
		*pBits->pPos++ |= pBits->currBits & mask;
		pBits->bytesLeft--;
		pBits->hasBits = 0;
		pBits->currBits = 0;
	}
}

//-----------------------------------------------------------------------------------------------------------
inline void BsWriteFlushMsb(BitStreamOut *pBits) {
	BsWriteMsb(pBits, 0, 0);
	if (pBits->hasBits) {
		unsigned char invmask = (1 << (8 - pBits->hasBits)) - 1;
		*pBits->pPos = *pBits->pPos & invmask;
		*pBits->pPos++ |= (pBits->currBits >> 24) & ~invmask;
		pBits->bytesLeft--;
		pBits->hasBits = 0;
		pBits->currBits = 0;
	}
}

//-----------------------------------------------------------------------------------------------------------
template <class BitStream> inline void BsSetPosLsb(BitStream *pBits, unsigned int bitPos) {
	pBits->bytesLeft = pBits->bytesLeft + (pBits->pPos - pBits->pBase) - ((bitPos + 7) >> 3);
	pBits->pPos = pBits->pBase + (bitPos >> 3);
	pBits->hasBits = (8 - (bitPos & 7)) & 7;
	pBits->currBits = 0;
	if (pBits->hasBits)
		pBits->currBits = *pBits->pPos++ >> (bitPos & 7);
}

//-----------------------------------------------------------------------------------------------------------
template <class BitStream> inline void BsSetPosMsb(BitStream *pBits, unsigned int bitPos) {
	pBits->bytesLeft = pBits->bytesLeft + (pBits->pPos - pBits->pBase) - ((bitPos + 7) >> 3);
	pBits->pPos = pBits->pBase + (bitPos >> 3);
	pBits->hasBits = (8 - (bitPos & 7)) & 7;
	pBits->currBits = 0;
	if (pBits->hasBits)
		pBits->currBits = *pBits->pPos++ << (24 + (bitPos & 7));
}

//-----------------------------------------------------------------------------------------------------------
template <class BitStream> inline void BsSkipLsb(BitStream *pBits, int numBits) {
	int curr = static_cast<int>(((pBits->pPos - pBits->pBase) << 3) + pBits->hasBits);
	BsSetPosLsb(pBits, static_cast<unsigned int>(curr + numBits));
}

//-----------------------------------------------------------------------------------------------------------
template <class BitStream> inline void BsSkipMsb(BitStream *pBits, int numBits) {
	int curr = static_cast<int>(((pBits->pPos - pBits->pBase) << 3) + pBits->hasBits);
	BsSetPosMsb(pBits, static_cast<unsigned int>(curr + numBits));
}

//-----------------------------------------------------------------------------------------------------------
template <class BitStream> inline void BsUndoLsb(BitStream *pBits, unsigned int in, unsigned char numBits) {
	pBits->currBits <<= numBits;
	pBits->currBits |= in & ((1 << numBits) - 1);
	pBits->hasBits += numBits;
}

//-----------------------------------------------------------------------------------------------------------
template <class BitStream> inline void BsUndoMsb(BitStream *pBits, unsigned int in, unsigned char numBits) {
	pBits->currBits >>= numBits;
	pBits->currBits |= in << (32 - numBits);
	pBits->hasBits += numBits;
}

//-------------------------------------------------------------------------------------------------------
inline unsigned char BitReverse8(unsigned char value) {
	return static_cast<unsigned char>(((value * 0x0802u & 0x22110u) | (value * 0x8020u & 0x88440u)) * 0x10101u >> 16);
}

//-------------------------------------------------------------------------------------------------------
inline unsigned short BitReverse16(unsigned short value) {
	value = ((value & 0xaaaa) >> 1) | ((value & 0x5555) << 1);
	value = ((value & 0xcccc) >> 2) | ((value & 0x3333) << 2);
	value = ((value & 0xf0f0) >> 4) | ((value & 0x0f0f) << 4);
	return (value >> 8) | (value << 8);
}

//-------------------------------------------------------------------------------------------------------
inline unsigned int BitReverse32(unsigned int value) {
	value = ((value & 0xaaaaaaaa) >> 1) | ((value & 0x55555555) << 1);
	value = ((value & 0xcccccccc) >> 2) | ((value & 0x33333333) << 2);
	value = ((value & 0xf0f0f0f0) >> 4) | ((value & 0x0f0f0f0f) << 4);
	value = ((value & 0xff00ff00) >> 8) | ((value & 0x00ff00ff) << 8);
	return (value >> 16) | (value << 16);
}

//-------------------------------------------------------------------------------------------------------
inline int BitGetCount(unsigned int value) {
	value = value - ((value >> 1) & 0x55555555);
	value = (value & 0x33333333) + ((value >> 2) & 0x33333333);
	return ((value + (value >> 4) & 0xf0f0f0f0) * 0x01010101) >> 24;
}

//-------------------------------------------------------------------------------------------------------
inline int BitGetMsb(unsigned int value) {
	static const int MultiplyDeBruijnBitPosition[32] = { 0, 9, 1, 10, 13, 21, 2, 29,
		11, 14, 16, 18, 22, 25, 3, 30, 8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31 };
	value |= value >> 1;
	value |= value >> 2;
	value |= value >> 4;
	value |= value >> 8;
	value |= value >> 16;
	return MultiplyDeBruijnBitPosition[(value * 0x07c4acddu) >> 27];
}

//-------------------------------------------------------------------------------------------------------
inline int BitGetLsb(unsigned int value) {
	static const int MultiplyDeBruijnBitPosition[32] = { 0, 1, 28, 2, 29, 14, 24, 3,
		30, 22, 20, 15, 25, 17, 4, 8, 31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9 };
	return MultiplyDeBruijnBitPosition[((value & -static_cast<int>(value)) * 0x077cb531u) >> 27];
}

//-------------------------------------------------------------------------------------------------------
inline unsigned short ByteSwap16(unsigned short value) { 
	value = (value >> 8) | (value << 8);
	return value;
}

//-------------------------------------------------------------------------------------------------------
inline unsigned int ByteSwap32(unsigned int value) {
	value = ((value & 0xff00ff00) >> 8) | ((value & 0x00ff00ff) << 8);
	value = (value >> 16) | (value << 16);
	return value;
}

//-------------------------------------------------------------------------------------------------------
inline unsigned long long ByteSwap64(unsigned long long value) {
	value = ((value & 0xff00ff00ff00ff00ull) >> 8) | ((value & 0x00ff00ff00ff00ffull) << 8);
	value = ((value & 0xffff0000ffff0000ull) >> 16) | ((value & 0x0000ffff0000ffffull) << 16);
	value = (value >> 32) | (value << 32);
	return value;
}

}  // namespace

#endif  // _SAW_IO_H_INCLUDED



#ifdef SAW_IO_IMPLEMENTATION

#include <stdarg.h>             // va_list
#include <stdio.h>              // fopen and family
#include <string.h>             // strncmp/_strnicmp
#include <memory.h>             // memcpy
#ifdef _WIN32
#	include <windows.h>         // many windows-specific functions
#else
#	ifdef __APPLE__
#		include <mach-o/dyld.h> // _NSGetExecutablePath
#	endif
#	include <unistd.h>          // readlink, getcwd, chdir
#	include <sys/types.h>       // This has to precede sys/stat.h
#	include <sys/stat.h>        // stat
#	include <ftw.h>             // recursive folder walk
#	include <stdlib.h>          // getenv, realpath
#endif

namespace saw {

static const int FS_PATH_MAX_LEN = 2048;

#ifdef _WIN32
static std::wstring utf8to16(const std::string &utf8) {
	std::wstring utf16;
	utf16.resize(FS_PATH_MAX_LEN);
	int size = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), static_cast<int>(utf8.size()), &utf16[0], FS_PATH_MAX_LEN);
	utf16.resize(size);
	return std::move(utf16);
}

static std::string utf16to8(const std::wstring &utf16) {
	std::string utf8;
	utf8.resize(FS_PATH_MAX_LEN);
	int size = WideCharToMultiByte(CP_UTF8, 0, utf16.c_str(), static_cast<int>(utf16.size()), &utf8[0], FS_PATH_MAX_LEN, 0, 0);
	utf8.resize(size);
	return std::move(utf8);
}
#endif

//-----------------------------------------------------------------------------------------------------------
bool IoOpenFile(Io *io, const std::string &filename, IoAccessType access) {
	if (!io)
		return false;
#ifdef _WIN32
	std::wstring utf16 = std::move(utf8to16(filename));
	switch (access) {
	case IO_ACCESS_RW_NEW: _wfopen_s(reinterpret_cast<FILE **>(&io->handle), utf16.c_str(), L"w+b"); break;
	case IO_ACCESS_RW: _wfopen_s(reinterpret_cast<FILE **>(&io->handle), utf16.c_str(), L"r+b"); if (!io->handle) _wfopen_s(reinterpret_cast<FILE **>(&io->handle), utf16.c_str(), L"w+b"); break;
	case IO_ACCESS_R: _wfopen_s(reinterpret_cast<FILE **>(&io->handle), utf16.c_str(), L"rb"); break;
	default: return false;
	}
#else
	switch (access) {
	case IO_ACCESS_RW_NEW: io->handle = fopen(filename.c_str(), "w+b"); break;
	case IO_ACCESS_RW: io->handle = fopen(filename.c_str(), "r+b"); if (!io->handle) io->handle = fopen(filename.c_str(), "w+b"); break;
	case IO_ACCESS_R: io->handle = fopen(filename.c_str(), "rb"); break;
	default: return false;
	}
#endif

	if (!io->handle)
		return false;

	io->funcRead = [](void *handle, size_t bytes, void *pOut) -> bool {
		return fread(pOut, 1, bytes, reinterpret_cast<FILE *>(handle)) == bytes;
	};
	io->funcWrite = [](void *handle, size_t bytes, const void *pIn) -> bool {
		return fwrite(pIn, 1, bytes, reinterpret_cast<FILE *>(handle)) == bytes;
	};
	io->funcSeek = [](void *handle, IoSeekType type, long long offset) -> bool {
		int from = SEEK_SET;
		if (type == IO_SEEK_REL)
			from = SEEK_CUR;
		if (type == IO_SEEK_END)
			from = SEEK_END;
#ifdef _WIN32
		return _fseeki64(reinterpret_cast<FILE *>(handle), offset, from) == 0;
#else
		return fseeko64(reinterpret_cast<FILE *>(handle), static_cast<off64_t>(offset), from) == 0;
#endif
	};
	io->funcTell = [](void *handle) -> long long {
		return ftell(reinterpret_cast<FILE *>(handle));
	};
	io->funcClose = [](void *handle) {
		fclose(reinterpret_cast<FILE *>(handle));
	};

	return true;
}

//-----------------------------------------------------------------------------------------------------------
bool IoOpenMem(Io *io, const void *pMem, size_t size) {
	struct MemHandle {
		char *pBase;
		char *pCurr;
		char *pEnd;
	};

	if (!io)
		return false;
	MemHandle *pH = new MemHandle;
	pH->pBase = reinterpret_cast<char *>(const_cast<void *>(pMem));
	pH->pCurr = pH->pBase;
	pH->pEnd = pH->pBase + size;

	io->handle = pH;

	if (!io->handle)
		return false;

	io->funcRead = [](void *handle, size_t bytes, void *pOut) -> bool {
		MemHandle *pMem = reinterpret_cast<MemHandle *>(handle);
		if (pMem->pCurr + bytes > pMem->pEnd)
			return false;
		memcpy(pOut, pMem->pCurr, bytes);
		pMem->pCurr += bytes;
		return true;
	};
	io->funcWrite = [](void *handle, size_t bytes, const void *pIn) -> bool {
		MemHandle *pMem = reinterpret_cast<MemHandle *>(handle);
		if (pMem->pCurr + bytes > pMem->pEnd)
			return false;
		memcpy(pMem->pCurr, pIn, bytes);
		pMem->pCurr += bytes;
		return true;
	};
	io->funcSeek = [](void *handle, IoSeekType type, long long offset) -> bool {
		MemHandle *pMem = reinterpret_cast<MemHandle *>(handle);
		switch (type) {
		case IO_SEEK_SET:
			if (pMem->pBase + offset > pMem->pEnd || offset < 0)
				return false;
			pMem->pCurr = pMem->pBase + offset;
			break;
		case IO_SEEK_REL:
			if (pMem->pCurr + offset > pMem->pEnd || pMem->pCurr + offset < pMem->pBase)
				return false;
			pMem->pCurr += offset;
			break;
		case IO_SEEK_END:
			if (pMem->pEnd - offset < pMem->pBase || offset < 0)
				return false;
			pMem->pCurr = pMem->pEnd - offset;
			break;
		}
		return true;
	};
	io->funcTell = [](void *handle) -> long long {
		MemHandle *pMem = reinterpret_cast<MemHandle *>(handle);
		return pMem->pCurr - pMem->pBase;
	};
	io->funcClose = [](void *handle) {
		delete reinterpret_cast<MemHandle *>(handle);
	};

	return true;
}

//-----------------------------------------------------------------------------------------------------------
#ifndef SAW_IO_NO_VECTOR
bool IoOpenVec(Io *io, std::vector<char> *pVec) {
	struct VecHandle {
		std::vector<char> *pVec;
		size_t pos;
	};

	if (!io)
		return false;
	VecHandle *pH = new VecHandle;
	pH->pVec = pVec;
	pH->pos = 0;

	io->handle = pH;

	if (!io->handle)
		return false;

	io->funcRead = [](void *handle, size_t bytes, void *pOut) -> bool {
		VecHandle *pVec = reinterpret_cast<VecHandle *>(handle);
		if (pVec->pos + bytes > pVec->pVec->size())
			return false;
		memcpy(pOut, &pVec->pVec->at(pVec->pos), bytes);
		pVec->pos += bytes;
		return true;
	};
	io->funcWrite = [](void *handle, size_t bytes, const void *pIn) -> bool {
		VecHandle *pVec = reinterpret_cast<VecHandle *>(handle);
		if (pVec->pos + bytes > pVec->pVec->size())
			pVec->pVec->resize(pVec->pos + bytes);
		memcpy(&pVec->pVec->at(pVec->pos), pIn, bytes);
		pVec->pos += bytes;
		return true;
	};
	io->funcSeek = [](void *handle, IoSeekType type, long long offset) -> bool {
		VecHandle *pVec = reinterpret_cast<VecHandle *>(handle);
		switch (type) {
		case IO_SEEK_SET:
			if (offset > static_cast<long long>(pVec->pVec->size()) || offset < 0)
				return false;
			pVec->pos = static_cast<size_t>(offset);
			break;
		case IO_SEEK_REL:
			if (static_cast<long long>(pVec->pos) + offset > static_cast<long long>(pVec->pVec->size()) || pVec->pos + offset < 0)
				return false;
			pVec->pos = static_cast<size_t>(static_cast<long long>(pVec->pos) + offset);
			break;
		case IO_SEEK_END:
			if (offset > static_cast<long long>(pVec->pVec->size()) || offset < 0)
				return false;
			pVec->pos = static_cast<size_t>(static_cast<long long>(pVec->pVec->size()) - offset);
			break;
		}
		return true;
	};
	io->funcTell = [](void *handle) -> long long {
		VecHandle *pVec = reinterpret_cast<VecHandle *>(handle);
		return static_cast<long long>(pVec->pos);
	};
	io->funcClose = [](void *handle) {
		delete reinterpret_cast<VecHandle *>(handle);
	};

	return true;
}
#endif

//-----------------------------------------------------------------------------------------------------------
void IoClose(Io *io) {
	if (io && io->handle) {
		io->funcClose(io->handle);
		io->handle = 0;
		io->funcRead = 0;
		io->funcWrite = 0;
		io->funcSeek = 0;
		io->funcTell = 0;
		io->funcClose = 0;
	}
}

//-----------------------------------------------------------------------------------------------------------
bool IoReadText8(const Io *io, char *buf, int bufSize) {
	if (bufSize <= 0)
		return true;
	bufSize--;  // Remove one because we need to ensure null terminator fits
	bool wasRead = true;
	while (bufSize > 0) {
		wasRead = io->funcRead(io->handle, 1, buf);
		if (!wasRead || !*buf)
			break;
		buf++;
		bufSize--;
	}
	*buf = 0;
	return wasRead;
}

//-----------------------------------------------------------------------------------------------------------
bool IoReadLine8(const Io *io, char *buf, int bufSize) {
	if (bufSize <= 0)
		return false;
	bufSize--;  // Remove one because we need to ensure null terminator fits
	bool wasRead = true;
	while (bufSize > 0) {
		wasRead = io->funcRead(io->handle, 1, buf);
		if (!wasRead || !*buf)
			break;
		if (*buf == '\r' || *buf == '\n') {
			char c2 = 0;
			if (io->funcRead(io->handle, 1, &c2)) {
				if ((*buf == '\r' && c2 != '\n') || (*buf == '\n' && c2 != '\r'))
					IoSeek(io, IO_SEEK_REL, -1);
			}
			break;
		}
		buf++;
		bufSize--;
	}
	*buf = 0;
	return wasRead;
}

//-----------------------------------------------------------------------------------------------------------
// len = -1 reads until null character or end of stream
bool IoReadText8(const Io *io, std::string *pOut, int len) {
	char buf[256];
	int i = 0;
	*pOut = "";
	bool wasRead = true;
	while (len != 0) {
		wasRead = io->funcRead(io->handle, 1, &buf[i]);
		if (!wasRead || !buf[i])
			break;
		i++;
		if (i == 255) {
			buf[i] = 0;
			i = 0;
			*pOut += buf;
		}
		len--;
	}
	buf[i] = 0;
	*pOut += buf;
	return wasRead;
}

//-----------------------------------------------------------------------------------------------------------
bool IoReadLine8(const Io *io, std::string *pOut) {
	// 256 would be fine, but we'll need 1 extra for if we hit EOL and need to read 1 extra character
	char buf[257];
	int i = 0;
	*pOut = "";
	bool wasRead = true;
	while (1) {
		wasRead = io->funcRead(io->handle, 1, &buf[i]);
		if (!wasRead || !buf[i])
			break;
		if (buf[i] == '\r' || buf[i] == '\n') {
			i++;
			if (io->funcRead(io->handle, 1, &buf[i])) {
				if ((buf[i - 1] == '\r' && buf[i] != '\n') || (buf[i - 1] == '\n' && buf[i] != '\r')) {
					i--;
					IoSeek(io, IO_SEEK_REL, -1);
				}
				i++;
			}
			break;
		}
		i++;
		if (i == 255) {
			buf[i] = 0;
			i = 0;
			*pOut += buf;
		}
	}
	buf[i] = 0;
	*pOut += buf;
	return wasRead;
}

//-----------------------------------------------------------------------------------------------------------
void IoWriteSVle(const Io *io, const char *format, va_list v) {
	while (*format) {
		switch (*format++) {
			case ' ': break;
			case '1': { int *x = va_arg(v, int *); IoWrite8(io, *x); break; }
			case '2': { int *x = va_arg(v, int *); IoWrite16le(io, *x); break; }
			case '4': { int *x = va_arg(v, int *); IoWrite32le(io, *x); break; }
			case 'f': case 'F': { float *x = va_arg(v, float *); IoWriteFle(io, *x); break; }
			case 'd': case 'D': { double *x = va_arg(v, double *); IoWriteDle(io, *x); break; }
			default: va_end(v); return;
		}
	}
}

//-----------------------------------------------------------------------------------------------------------
void IoWriteSle(const Io *io, const char *format, ...) {
	va_list v;
	va_start(v, format);
	IoWriteSVle(io, format, v);
	va_end(v);
}

//-----------------------------------------------------------------------------------------------------------
void IoWriteSVbe(const Io *io, const char *format, va_list v) {
	while (*format) {
		switch (*format++) {
			case ' ': break;
			case '1': { int *x = va_arg(v, int *); IoWrite8(io, *x); break; }
			case '2': { int *x = va_arg(v, int *); IoWrite16be(io, *x); break; }
			case '4': { int *x = va_arg(v, int *); IoWrite32be(io, *x); break; }
			case 'f': case 'F': { float *x = va_arg(v, float *); IoWriteFbe(io, *x); break; }
			case 'd': case 'D': { double *x = va_arg(v, double *); IoWriteDbe(io, *x); break; }
			default: return;
		}
	}
}

//-----------------------------------------------------------------------------------------------------------
void IoWriteSbe(const Io *io, const char *format, ...) {
	va_list v;
	va_start(v, format);
	IoWriteSVbe(io, format, v);
	va_end(v);
}

//-----------------------------------------------------------------------------------------------------------
void IoWriteText8(const Io *io, const char *in, bool includeNull) {
	IoWriteRaw(io, strlen(in) + (includeNull ? 1 : 0), in);
}

//-----------------------------------------------------------------------------------------------------------
void IoWriteLine8(const Io *io, const char *in) {
	size_t bytes = strlen(in);
	IoWriteRaw(io, bytes, in);
	if (in[bytes - 1] != '\r' && in[bytes - 1] != '\n') {
		IoWrite8(io, '\r'); 
		IoWrite8(io, '\n');
	}
}

//-----------------------------------------------------------------------------------------------------------
void BitInvertBuf(void *pData, size_t numBytes) {
	unsigned int *pData32 = reinterpret_cast<unsigned int *>(pData);
	for (size_t i = numBytes >> 2; i--; pData32++)
		*pData32 = ~(*pData32);
	unsigned char *pData8 = reinterpret_cast<unsigned char *>(pData32);
	for (size_t i = numBytes & 3; i--; pData8++)
		*pData8 = ~(*pData8);
}

//-----------------------------------------------------------------------------------------------------------
void BitReverseBuf8(void *pData, size_t numBytes) {
	unsigned int *pData32 = reinterpret_cast<unsigned int *>(pData);
	for (size_t i = numBytes >> 2; i--; pData32++) {  // Reverse 4 bytes at a time in parallel
		*pData32 = ((*pData32 & 0xaaaaaaaa) >> 1) | ((*pData32 & 0x55555555) << 1);
		*pData32 = ((*pData32 & 0xcccccccc) >> 2) | ((*pData32 & 0x33333333) << 2);
		*pData32 = ((*pData32 & 0xf0f0f0f0) >> 4) | ((*pData32 & 0x0f0f0f0f) << 4);
	}
	unsigned char *pData8 = reinterpret_cast<unsigned char *>(pData32);
	for (size_t i = numBytes & 3; i--; pData8++)
		*pData8 = BitReverse8(*pData8);
}

//-----------------------------------------------------------------------------------------------------------
void ByteSwapBuf16(void *pData, size_t count) {
	unsigned short *pData16 = reinterpret_cast<unsigned short *>(pData);
	for (size_t i = 0; i < count; i++)
		ByteSwap16(pData16[i]);
}

//-----------------------------------------------------------------------------------------------------------
void ByteSwapBuf32(void *pData, size_t count) {
	unsigned int *pData32 = reinterpret_cast<unsigned int *>(pData);
	for (size_t i = 0; i < count; i++)
		ByteSwap32(pData32[i]);
}

//-----------------------------------------------------------------------------------------------------------
bool FsCreateDir(const std::string &fullPathDir) {
#ifdef _WIN32
	// http://mfctips.com/2012/03/26/best-way-to-check-if-file-or-directory-exists/
	std::wstring utf16 = utf8to16(fullPathDir);
	DWORD attrib = GetFileAttributesW(utf16.c_str());
	DWORD err = GetLastError();
	if (attrib == INVALID_FILE_ATTRIBUTES && (err == ERROR_PATH_NOT_FOUND || err == ERROR_FILE_NOT_FOUND))
#else
	struct stat data;
	if (stat(fullPathDir.c_str(), &data) != 0)
#endif
	{
		size_t index = fullPathDir.find_last_of("/\\");
		if (index == fullPathDir.length() - 1)
			index = fullPathDir.find_last_of("/\\", index - 1);
		if (index != std::string::npos && !FsCreateDir(std::move(fullPathDir.substr(0, index))))
			return false;
#ifdef _WIN32
		if (!CreateDirectoryW(utf16.c_str(), NULL))
			return false;
#else
		if (mkdir(fullPathDir.c_str(), S_IRWXU) != 0)
			return false;
#endif
	}
	return true;
}

//-----------------------------------------------------------------------------------------------------------
bool FsDeleteDir(const std::string &fullPathDir) {
#ifdef _WIN32
	auto funcDel = [](const std::string &fullPath, bool isFile, void * /*user*/) -> bool {
		std::wstring utf16 = std::move(utf8to16(fullPath));

		// http://stackoverflow.com/questions/3764072/c-win32-how-to-wait-for-a-pending-delete-to-complete
		// Looks like a problem specific to Windows
		MoveFileExW(utf16.c_str(), (utf16 + L".del.me").c_str(), MOVEFILE_WRITE_THROUGH);
		utf16 += L".del.me";

		if (isFile)
			return DeleteFileW(utf16.c_str()) != 0;

		return RemoveDirectoryW(utf16.c_str()) != 0;
	};
#else
	auto funcDel = [](const std::string &fullPath, bool isFile, void * /*user*/) -> bool {
		if (isFile)
			return unlink(fullPath.c_str()) == 0;
		return rmdir(utf16.c_str()) == 0;
	};
#endif
	return FsWalk(fullPathDir, FS_WALK_DEPTH, funcDel, 0);
}

//-----------------------------------------------------------------------------------------------------------
bool FsDeleteFile(const std::string &fullPathFile) {
#ifdef _WIN32
	return DeleteFileW(utf8to16(fullPathFile).c_str()) != 0;
#else
	return unlink(fullPathFile.c_str()) == 0;
#endif
}

//-----------------------------------------------------------------------------------------------------------
bool FsRename(const std::string &fullPathOld, const std::string &fullPathNew) {
#ifdef _WIN32
	return MoveFileW(utf8to16(fullPathOld).c_str(), utf8to16(fullPathNew).c_str()) != 0;
#else
	return rename(fullPathOld.c_str(), fullPathNew.c_str()) != -1;
#endif
}

//-----------------------------------------------------------------------------------------------------------
bool FsSetWorkingDir(const std::string &fullPathDir) {
#ifdef _WIN32
	return SetCurrentDirectoryW(utf8to16(fullPathDir).c_str()) != 0;
#else
	return chdir(fullPathDir.c_str()) != -1;
#endif
}

//-----------------------------------------------------------------------------------------------------------
bool FsIsDir(const std::string &fullPath) {
#ifdef _WIN32
	DWORD attrib = GetFileAttributesW(utf8to16(fullPath).c_str());
	return attrib != INVALID_FILE_ATTRIBUTES && (attrib & FILE_ATTRIBUTE_DIRECTORY);
#else
	struct stat data;
	return stat(fullPath.c_str(), &data) == 0 && (data.st_mode & __S_IFDIR);
#endif
}

//-----------------------------------------------------------------------------------------------------------
bool FsIsFile(const std::string &fullPath) {
#ifdef _WIN32
	DWORD attrib = GetFileAttributesW(utf8to16(fullPath).c_str());
	return attrib != INVALID_FILE_ATTRIBUTES && !(attrib & FILE_ATTRIBUTE_DIRECTORY);
#else
	struct stat data;
	return stat(fullPath.c_str(), &data) == 0 && (data.st_mode & __S_IFREG);
#endif
}

//-----------------------------------------------------------------------------------------------------------
bool FsIsReadOnly(const std::string &fullPath) {
#ifdef _WIN32
	DWORD attrib = GetFileAttributesW(utf8to16(fullPath).c_str());
	return attrib != INVALID_FILE_ATTRIBUTES && (attrib & FILE_ATTRIBUTE_READONLY);
#else
	struct stat data;
	return stat(fullPath.c_str(), &data) == 0 && access(fullPath.c_str(), W_OK) == -1;
#endif
}

//-----------------------------------------------------------------------------------------------------------
bool FsIsHidden(const std::string &fullPath) {
#ifdef _WIN32
	DWORD attrib = GetFileAttributesW(utf8to16(fullPath).c_str());
	return attrib != INVALID_FILE_ATTRIBUTES && (attrib & FILE_ATTRIBUTE_HIDDEN);
#else
	// Linux hidden files simply begin with a .
	// Need extension detection in case the file name is something like .abcdefg
	std::string file = FsGetPathFile(fullPath, true);
	return !file.empty() && file.at(0) == '.';
#endif
}

//-----------------------------------------------------------------------------------------------------------
bool FsIsRelative(const std::string &path) {
	const char *buf = path.c_str();
	if (buf && buf[0] == '.') {
		int n = buf[1] == '.' ? 2 : 1;
		return buf[n] == '/' || buf[n] == '\\';
	}
	return false;
}

//-----------------------------------------------------------------------------------------------------------
time_t FsGetTimeAccess(const std::string &fullPath) {
#ifdef _WIN32
	WIN32_FILE_ATTRIBUTE_DATA data;
	if (!GetFileAttributesExW(utf8to16(fullPath).c_str(), GetFileExInfoStandard, &data))
		return 0;
	unsigned long long hns1601 = (static_cast<unsigned long long>(data.ftLastAccessTime.dwHighDateTime) << 32) | data.ftLastAccessTime.dwLowDateTime;
	return static_cast<time_t>(hns1601 / 10000000ULL - 11644473600ULL);
#else
	struct stat data;
	stat(fullPath.c_str(), &data);
	return data.st_atime;
#endif
}

//-----------------------------------------------------------------------------------------------------------
time_t FsGetTimeMod(const std::string &fullPath) {
#ifdef _WIN32
	WIN32_FILE_ATTRIBUTE_DATA data;
	if (!GetFileAttributesExW(utf8to16(fullPath).c_str(), GetFileExInfoStandard, &data))
		return 0;
	unsigned long long hns1601 = (static_cast<unsigned long long>(data.ftLastWriteTime.dwHighDateTime) << 32) | data.ftLastWriteTime.dwLowDateTime;
	return static_cast<time_t>(hns1601 / 10000000ULL - 11644473600ULL);
#else
	struct stat data;
	stat(fullPath.c_str(), &data);
	return data.st_mtime;
#endif
}

//-----------------------------------------------------------------------------------------------------------
time_t FsGetTimeCreate(const std::string &fullPath) {
#ifdef _WIN32
	WIN32_FILE_ATTRIBUTE_DATA data;
	if (!GetFileAttributesExW(utf8to16(fullPath).c_str(), GetFileExInfoStandard, &data))
		return 0;
	unsigned long long hns1601 = (static_cast<unsigned long long>(data.ftCreationTime.dwHighDateTime) << 32) | data.ftCreationTime.dwLowDateTime;
	return static_cast<time_t>(hns1601 / 10000000ULL - 11644473600ULL);
#else
	struct stat data;
	stat(fullPath.c_str(), &data);
	return data.st_ctime;
#endif
}

//-----------------------------------------------------------------------------------------------------------
std::string FsGetTempDir(bool includeSlash) {
#ifdef _WIN32
	wchar_t path[FS_PATH_MAX_LEN];
	GetTempPathW(FS_PATH_MAX_LEN, path);
	return std::move(utf16to8(path) + (includeSlash ? "\\" : ""));
#else
	const char *path = getenv("TMPDIR");
	if (path == 0)
		path = "/tmp";
	return std::move(std::string(path) + (includeSlash ? "/" : ""));
#endif
}

//-----------------------------------------------------------------------------------------------------------
std::string FsGetWorkingDir(bool includeSlash) {
#ifdef _WIN32
	wchar_t path[FS_PATH_MAX_LEN];
	GetCurrentDirectoryW(FS_PATH_MAX_LEN, path);
	return std::move(utf16to8(path) + (includeSlash ? "\\" : ""));
#else
	char path[FS_PATH_MAX_LEN];
	getcwd(path, FS_PATH_MAX_LEN);
	return std::move(std::string(path) + (includeSlash ? "/" : ""));
#endif
}

//-----------------------------------------------------------------------------------------------------------
std::string FsGetAppDir(bool includeSlash) {
	return std::move(FsGetPathDir(FsGetAppPath(), includeSlash));
}

//-----------------------------------------------------------------------------------------------------------
std::string FsGetAppPath() {
#ifdef _WIN32
	wchar_t path[FS_PATH_MAX_LEN];
	DWORD len = GetModuleFileNameW(NULL, path, FS_PATH_MAX_LEN);
	if (len <= 0 || len == FS_PATH_MAX_LEN)
		path[0] = 0;
	return std::move(utf16to8(path));
#elif defined(__linux) || defined(__NetBSD__) || defined(__OpenBSD__)
	char path[FS_PATH_MAX_LEN];
	char link[32];
#	ifdef __linux
	snprintf(link, sizeof(link), "/proc/%d/exe", getpid());
#	else
	snprintf(link, sizeof(link), "/proc/%d/file", getpid());
#	endif
	ssize_t len = readlink(link, path, FS_PATH_MAX_LEN);
	if (len <= 0 || len == FS_PATH_MAX_LEN)
		path[0] = 0;
	path[len] = 0;  // readlink doesn't null terminate
	return path;
#elif defined(__FreeBSD__)
	char path[FS_PATH_MAX_LEN];
	int name[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1 };
	size_t len = FS_PATH_MAX_LEN - 1;
	if (sysctl(name, 4, path, &len, NULL, 0) != 0)
		path[0] = 0;
	return path;
#elif defined(__APPLE__)
	char path[FS_PATH_MAX_LEN];
	uint32_t bufSize = FS_PATH_MAX_LEN;
	if (_NSGetExecutablePath(path, &bufSize) != 0)
		path[0] = 0;
	return path;
#endif
}

//-----------------------------------------------------------------------------------------------------------
std::string FsGetRelPath(const std::string &fullPathFrom, const std::string &fullPathTo) {
	const char *bufFrom = fullPathFrom.c_str();
	const char *bufTo = fullPathTo.c_str();
	size_t lenFrom = fullPathFrom.size();
	size_t lenTo = fullPathTo.size();

	if (!lenFrom || !lenTo)
		return std::string();
#ifdef _WIN32
	else {
		// No relative path if the drives are different
		char lowFrom = bufFrom[0] >= 'A' && bufFrom[0] <= 'Z' ? bufFrom[0] | 0x60 : bufFrom[0];
		char lowTo = bufTo[0] >= 'A' && bufTo[0] <= 'Z' ? bufTo[0] | 0x60 : bufTo[0];
		if (lowFrom != lowTo)
			return std::string();
	}
	const char *dot = ".\\";
	const char *dotdot = "..\\";
	const char *delim = "\\";
#else
	const char *dot = "./";
	const char *dotdot = "../";
	const char *delim = "/";
#endif
	size_t posMatch = 0;
	size_t posFrom = 0;
	while (posFrom != lenFrom) {
		posFrom = fullPathFrom.find_first_of("\\/", posMatch);
		if (posFrom == std::string::npos)
			posFrom = lenFrom;

		if (posFrom > lenTo)
			break;

#ifdef _WIN32
		// Windows FS is case insensitive
		if (_strnicmp(bufFrom + posMatch, bufTo + posMatch, posFrom - posMatch) != 0)
			break;
#else
		// Non-windows FS is case sensitive
		if (strncmp(bufFrom + posMatch, bufTo + posMatch, posFrom - posMatch) != 0)
			break;
#endif

		posMatch = fullPathFrom.find_first_not_of("\\/", posFrom);
		if (posMatch == std::string::npos)
			posMatch = lenFrom;
	}

	std::string output;
	output.reserve(FS_PATH_MAX_LEN);
	if (posFrom == lenFrom) {
		output.assign(dot);
	}
	else {
		output.append(dotdot);
		while (posFrom != std::string::npos) {
			posFrom = fullPathFrom.find_first_not_of("\\/", posFrom);
			if (posFrom != std::string::npos) {
				posFrom = fullPathFrom.find_first_of("\\/", posFrom);
				output.append(dotdot);
			}
		}
	}

	posMatch = fullPathTo.find_first_not_of("\\/", posMatch);
	while (posMatch != std::string::npos) {
		size_t posTo = fullPathTo.find_first_of("\\/", posMatch);
		if (posTo == std::string::npos)
			posTo = lenTo;
		
		output.append(bufTo + posMatch, bufTo + posTo);

		posMatch = fullPathTo.find_first_not_of("\\/", posTo);
		if (posMatch == std::string::npos) {
			if (posTo != lenTo)
				output.append(delim);
		}
	};

	return std::move(output);
}

//-----------------------------------------------------------------------------------------------------------
std::string FsGetFullPath(const std::string &path) {
#ifdef _WIN32
	wchar_t out16[FS_PATH_MAX_LEN];
	GetFullPathNameW(utf8to16(path).c_str(), FS_PATH_MAX_LEN - 1, out16, 0);
	return std::move(utf16to8(out16));
#else
	char out[FS_PATH_MAX_LEN];
	realpath(path.c_str(), out);
	return std::move(std::string(out));
#endif
}

//-----------------------------------------------------------------------------------------------------------
std::string FsGetPathDir(const std::string &fullPath, bool includeSlash) {
	std::string dir;
	auto lastSlash = fullPath.find_last_of("/\\");
	if (lastSlash != std::string::npos)
		dir = fullPath.substr(0, lastSlash + (includeSlash ? 1 : 0));
	return std::move(dir);
}

//-----------------------------------------------------------------------------------------------------------
std::string FsGetPathFile(const std::string &fullPath, bool includeExt) {
	std::string file;
	auto lastDot = fullPath.find_last_of('.');
	auto fileStart = fullPath.find_last_of("/\\");
	if (lastDot != std::string::npos && fileStart != std::string::npos && lastDot < fileStart)
		lastDot = std::string::npos;
	if (fileStart != std::string::npos)
		fileStart++;
	else
		fileStart = 0;
	file = fullPath.substr(fileStart, (includeExt ? fullPath.length() : lastDot) - fileStart);
	return std::move(file);
}

//-----------------------------------------------------------------------------------------------------------
std::string FsGetPathExt(const std::string &fullPath, bool includeDot) {
	std::string ext;
	auto lastDot = fullPath.find_last_of('.');
	auto lastSlash = fullPath.find_last_of("/\\");
	if (lastDot != std::string::npos && (lastSlash == std::string::npos || lastDot > lastSlash)) {
		if (!includeDot)
			lastDot++;
		ext = fullPath.substr(lastDot, fullPath.length() - lastDot);
	}
	return std::move(ext);
}

//-----------------------------------------------------------------------------------------------------------
unsigned long long FsGetFileSize(const std::string &fullPathFile) {
#ifdef _WIN32
	WIN32_FILE_ATTRIBUTE_DATA data;
	if (!GetFileAttributesExW(utf8to16(fullPathFile).c_str(), GetFileExInfoStandard, &data))
		return 0;
	return (static_cast<unsigned long long>(data.nFileSizeHigh) << 32) | data.nFileSizeLow;
#else
	struct stat64 data;
	stat64(fullPathFile.c_str(), &data);
	return data.st_size;
#endif
}

//-----------------------------------------------------------------------------------------------------------
FsWatchType FsWatch(const std::string &fullPath, int *state) {
	time_t timeMod = FsGetTimeMod(fullPath);
#ifdef _WIN32
	std::wstring utf16 = utf8to16(fullPath);
	DWORD attrib = GetFileAttributesW(utf16.c_str());
	bool exists = attrib != INVALID_FILE_ATTRIBUTES;
#else
	struct stat data;
	bool exists = stat(fullPath.c_str(), &data) == 0;
#endif

	int hash = static_cast<int>(((timeMod >> 16) ^ timeMod) << 16);
	if (exists)
		hash |= 0x00008000;
	int oldState = *state;

	if (oldState != hash) {
		*state = hash;
		if (oldState) {
			bool oldExists = !!(oldState & 0x00008000);
			if (oldExists != exists)
				return exists ? FS_WATCH_CREATED : FS_WATCH_REMOVED;
			return FS_WATCH_CHANGED;
		}
	}
	return FS_WATCH_NONE;
}

//-----------------------------------------------------------------------------------------------------------
#ifndef _WIN32
static FsWalkFunc gFsWalkCallback;
static void *gFsWalkUser;
#endif

bool FsWalk(const std::string &fullPath, FsWalkType type, FsWalkFunc callback, void *user) {
#ifdef _WIN32
	if (type == FS_WALK_NORMAL) {
		if (!callback(fullPath, false, user))
			return false;
	}

	WIN32_FIND_DATAW fd;
	std::wstring fullPath16 = std::move(utf8to16(fullPath));

	HANDLE hEnum = FindFirstFileExW((fullPath16 + L"\\*").c_str(), FindExInfoBasic, &fd, FindExSearchNameMatch, 0, 0);
	if (hEnum == INVALID_HANDLE_VALUE)
		return false;

	while (1) {
		if (fd.cFileName[0] != L'.' || (fd.cFileName[1] != 0 && (fd.cFileName[1] != L'.' || fd.cFileName[2] != 0))) {
			bool isDir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) || (fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT);
			
			std::string nextPath = std::move(utf16to8(fullPath16 + L'\\' + fd.cFileName));
			if (isDir) {
				if (!FsWalk(nextPath, type, callback, user)) {
					FindClose(hEnum);
					return false;
				}
			}
			else if (!callback(nextPath, true, user)) {
				FindClose(hEnum);
				return false;
			}
		}
		if (FindNextFileW(hEnum, &fd) == FALSE) {
			if (GetLastError() != ERROR_NO_MORE_FILES) {
				FindClose(hEnum);
				return false;
			}
			break;
		}
	}

	FindClose(hEnum);

	if (type == FS_WALK_DEPTH)
		return callback(fullPath, false, user);

	return true;
#else
	gFsWalkCallback = callback;
	gFsWalkUser = user;
	auto funcFtw = [](const char *nextPath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) -> int {
		if (!gFsWalkCallback(nextPath, typeflag == FTW_F, gFsWalkUser))
			return -1;
		return 0;
	};
	int flags = FTW_PHYS | (type == FS_WALK_DEPTH ? FTW_DEPTH : 0);
	return nftw(fullPath.c_str(), funcFtw, 64, flags);
#endif
}

}  // namespace

#endif  // SAW_IO_IMPLEMENTATION
