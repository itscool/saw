// saw_io.h - Io abstraction including file and memory implementations
//          - Bit streaming
//          - Bit twiddling and byte swapping
//          - Cross platform file system manipulation
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
// - v1.02 - 03/17/16 - Merged with bit streaming, bit twiddling, byte swapping libraries
//                    - Merged with file system manipulation library
// - v1.01 - 03/17/16 - Fixed treat C-string like std::string in linux IoOpenFile
// - v1.00 - 03/12/16 - Initial release by Scott Williams

//-----------------------------------------------------------------------------------------------------------
// Notes

//-----------------------------------------------------------------------------------------------------------
// Todo
// @@ (Fs) Better error reporting (besides true/false)
// @@ (Fs) Hand-written code for Windows version of FsDeleteDir so we can handle errors
// @@ (Fs) FsGetRelPath(fullPathFrom, fullPathTo)
// @@ (Fs) FsEnum(fullPath)
// @@ (Fs) FsWatch(fullPath)

#ifndef _SAW_IO_H_INCLUDED
#define _SAW_IO_H_INCLUDED

#include <stdarg.h>  // va_list
#include <string>
#include <vector>

namespace saw {

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

// Io implementations
bool IoOpenFile(Io *io, const char *filename, IoAccessType access);
bool IoOpenMem(Io *io, const void *pMem, size_t size);
bool IoOpenVec(Io *io, std::vector<char> *pVec);
inline void IoClose(const Io *io);

// Io abstraction
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
void IoReadSVle(const Io *io, const char *format, va_list v);
void IoReadSle(const Io *io, const char *format, ...);
void IoReadSVbe(const Io *io, const char *format, va_list v);
void IoReadSbe(const Io *io, const char *format, ...);
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
void IoWriteSVle(const Io *io, const char *format, va_list v);
void IoWriteSle(const Io *io, const char *format, ...);
void IoWriteSVbe(const Io *io, const char *format, va_list v);
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
inline unsigned int BitSet(unsigned int bits, unsigned int mask);
inline unsigned int BitReset(unsigned int bits, unsigned int mask);
inline unsigned int BitFlip(unsigned int bits, unsigned int mask);
inline unsigned int BitToggle(unsigned int bits, unsigned int mask, bool toggle);
inline unsigned char BitReverse8(unsigned char value);
inline unsigned short BitReverse16(unsigned short value);
inline unsigned int BitReverse32(unsigned int value);
inline int BitGetCount(unsigned int value);
inline int BitGetMsb(unsigned int value);
inline int BitGetLsb(unsigned int value);
void BitInvertBuf(void *pData, unsigned int numBytes);
void BitReverseBuf8(void *pData, unsigned int numBytes);
inline void ByteSwap16(unsigned short &value);
inline void ByteSwap32(unsigned int &value);
inline void ByteSwap64(unsigned long long &value);
void ByteSwapBuf16(void *pData, unsigned int count); // count in words, not bytes
void ByteSwapBuf32(void *pData, unsigned int count); // count in double words, not bytes

// File system manipulation
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
std::string FsGetFullPath(const std::string &path);
std::string FsGetPathDir(const std::string &fullPath, bool includeSlash);
std::string FsGetPathFile(const std::string &fullPath, bool includeExt);
std::string FsGetPathExt(const std::string &fullPath, bool includeDot);
unsigned long long FsGetFileSize(const std::string &fullPathFile);

//-----------------------------------------------------------------------------------------------------------
inline void IoClose(Io *io) {
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

//-----------------------------------------------------------------------------------------------------------
inline unsigned int BitSet(unsigned int bits, unsigned int mask) { return bits | mask; }

//-------------------------------------------------------------------------------------------------------
inline unsigned int BitReset(unsigned int bits, unsigned int mask) { return bits & ~mask; }

//-------------------------------------------------------------------------------------------------------
inline unsigned int BitFlip(unsigned int bits, unsigned int mask) { return bits ^ mask; }

//-------------------------------------------------------------------------------------------------------
inline unsigned int BitToggle(unsigned int bits, unsigned int mask, bool toggle) {
	return toggle ? BitSet(bits, mask) : BitReset(bits, mask);
}

//-------------------------------------------------------------------------------------------------------
inline unsigned char BitReverse8(unsigned char value) {
	return static_cast<unsigned char>(((value * 0x0802ul & 0x22110ul) | (value * 0x8020ul & 0x88440ul)) * 0x10101ul >> 16);
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
inline void ByteSwap16(unsigned short &value) { value = (value >> 8) | (value << 8); }

//-------------------------------------------------------------------------------------------------------
inline void ByteSwap32(unsigned int &value) {
	value = ((value & 0xff00ff00) >> 8) | ((value & 0x00ff00ff) << 8);
	value = (value >> 16) | (value << 16);
}

//-------------------------------------------------------------------------------------------------------
inline void ByteSwap64(unsigned long long &value) {
	value = ((value & 0xff00ff00ff00ff00ull) >> 8) | ((value & 0x00ff00ff00ff00ffull) << 8);
	value = ((value & 0xffff0000ffff0000ull) >> 16) | ((value & 0x0000ffff0000ffffull) << 16);
	value = (value >> 32) | (value << 32);
}

}  // namespace

#endif  // _SAW_IO_H_INCLUDED



#ifdef SAW_IO_IMPLEMENTATION

#include <sys/types.h>      // This has to preceed sys/stat.h
#include <sys/stat.h>       // stat (or _wstat)
#include <stdio.h>          // fopen and family
#include <memory.h>         // memcpy
#ifdef _WIN32
#	include <windows.h>     // many windows-specific functions
#	include <codecvt>       // utf8 <-> utf16
#elif defined(__linux) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#	include <unistd.h>      // readlink(), amongst others
#	include <ftw.h>         // recursive folder walk
#	include <stdlib.h>
#	include <libgen.h>
#	include <dirent.h>
#elif defined(__APPLE__)
#	include <mach-o/dyld.h> // _NSGetExecutablePath
#endif

namespace saw {

bool IoOpenFile(Io *io, const char *filename, IoAccessType access) {
	if (!io)
		return false;
#ifdef _WIN32
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> utf16conv;
	std::wstring utf16 = utf16conv.from_bytes(filename);
	switch (access) {
	case IO_ACCESS_RW_NEW: _wfopen_s(reinterpret_cast<FILE **>(&io->handle), utf16.c_str(), L"w+b"); break;
	case IO_ACCESS_RW: _wfopen_s(reinterpret_cast<FILE **>(&io->handle), utf16.c_str(), L"r+b"); if (!io->handle) _wfopen_s(reinterpret_cast<FILE **>(&io->handle), utf16.c_str(), L"w+b"); break;
	case IO_ACCESS_R: _wfopen_s(reinterpret_cast<FILE **>(&io->handle), utf16.c_str(), L"rb"); break;
	default: return false;
	}
#else
	switch (access) {
	case IO_ACCESS_RW_NEW: io->handle = fopen(filename, "w+b"); break;
	case IO_ACCESS_RW: io->handle = fopen(filename, "r+b"); if (!io->handle) io->handle = fopen(filename, "w+b"); break;
	case IO_ACCESS_R: io->handle = fopen(filename, "rb"); break;
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

//-----------------------------------------------------------------------------------------------------------
void IoReadSVle(const Io *io, const char *format, va_list v) {
	while (*format) {
		switch (*format++) {
			case ' ': break;
			case '1': { int *x = va_arg(v, int *); *x = IoRead8(io); break; }
			case '2': { int *x = va_arg(v, int *); *x = IoRead16le(io); break; }
			case '4': { int *x = va_arg(v, int *); *x = IoRead32le(io); break; }
			case 'f': case 'F': { float *x = va_arg(v, float *); *x = IoReadFle(io); break; }
			case 'd': case 'D': { double *x = va_arg(v, double *); *x = IoReadDle(io); break; }
			default: va_end(v); return;
		}
	}
}

//-----------------------------------------------------------------------------------------------------------
void IoReadSle(const Io *io, const char *format, ...) {
   va_list v;
   va_start(v, format);
   IoReadSVle(io, format, v);
   va_end(v);
}

//-----------------------------------------------------------------------------------------------------------
void IoReadSVbe(const Io *io, const char *format, va_list v) {
	while (*format) {
		switch (*format++) {
			case ' ': break;
			case '1': { int *x = va_arg(v, int *); *x = IoRead8(io); break; }
			case '2': { int *x = va_arg(v, int *); *x = IoRead16be(io); break; }
			case '4': { int *x = va_arg(v, int *); *x = IoRead32be(io); break; }
			case 'f': case 'F': { float *x = va_arg(v, float *); *x = IoReadFbe(io); break; }
			case 'd': case 'D': { double *x = va_arg(v, double *); *x = IoReadDbe(io); break; }
			default: va_end(v); return;
		}
	}
}

//-----------------------------------------------------------------------------------------------------------
void IoReadSbe(const Io *io, const char *format, ...) {
   va_list v;
   va_start(v, format);
   IoReadSVbe(io, format, v);
   va_end(v);
}

//-----------------------------------------------------------------------------------------------------------
bool IoReadText8(const Io *io, char *buf, int bufSize) {
	if (bufSize <= 0)
		return false;
	bufSize--;  // Remove one because we need to ensure null terminator fits
	bool wasRead = false;
	if (bufSize != 0)
		wasRead = io->funcRead(io->handle, 1, buf);
	while (wasRead && *buf) {
		wasRead = false;
		buf++;
		bufSize--;
		if (bufSize != 0)
			wasRead = io->funcRead(io->handle, 1, buf);
	}
	*buf = 0;
	return wasRead;
}

//-----------------------------------------------------------------------------------------------------------
bool IoReadLine8(const Io *io, char *buf, int bufSize) {
	if (bufSize <= 0)
		return false;
	bufSize--;  // Remove one because we need to ensure null terminator fits
	bool wasRead = false;
	char c = 0;
	if (bufSize != 0)
		wasRead = io->funcRead(io->handle, 1, &c);
	while (wasRead && c) {
		if (c == 13 || c == 10) {
			char c2 = 0;
			if (io->funcRead(io->handle, 1, &c2)) {
				if ((c == '\r' && c2 != '\n') || (c == '\n' && c2 != '\r'))
					IoSeek(io, IO_SEEK_REL, -1);
			}
			break;
		}

		wasRead = false;
		*buf = c;
		buf++;
		bufSize--;
		if (bufSize != 0)
			wasRead = io->funcRead(io->handle, 1, &c);
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
	bool wasRead = io->funcRead(io->handle, 1, &buf[i]);
	while (wasRead && buf[i] && len != 0) {
		i++;
		if (i == 255) {
			buf[i] = 0;
			i = 0;
			*pOut += buf;
		}
		len--;
		if (len != 0)
			wasRead = io->funcRead(io->handle, 1, &buf[i]);
	}
	buf[i] = 0;
	*pOut += buf;
	return wasRead || i > 0;
}

//-----------------------------------------------------------------------------------------------------------
bool IoReadLine8(const Io *io, std::string *pOut) {
	// 256 would be fine, but we'll need 1 extra for if we hit EOL and need to read 1 extra character
	char buf[257];
	int i = 0;
	*pOut = "";
	bool wasRead = io->funcRead(io->handle, 1, &buf[i]);
	while (wasRead && buf[i]) {
		if (buf[i] == 10 || buf[i] == 13) {
			i++;
			if (io->funcRead(io->handle, 1, &buf[i])) {
				if ((buf[i - 1] == 10 && buf[i] != 13) || (buf[i - 1] == 13 && buf[i] != 10)) {
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
		wasRead = io->funcRead(io->handle, 1, &buf[i]);
	}
	buf[i] = 0;
	*pOut += buf;
	return wasRead || i > 0;
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
			default: va_end(v); return;
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
void BitInvertBuf(void *pData, unsigned int numBytes) {
	unsigned int *pData32 = reinterpret_cast<unsigned int *>(pData);
	for (unsigned int i = numBytes >> 2; i--; pData32++)
		*pData32 = ~(*pData32);
	unsigned char *pData8 = reinterpret_cast<unsigned char *>(pData32);
	for (unsigned int i = numBytes & 3; i--; pData8++)
		*pData8 = ~(*pData8);
}

//-----------------------------------------------------------------------------------------------------------
void BitReverseBuf8(void *pData, unsigned int numBytes) {
	unsigned int *pData32 = reinterpret_cast<unsigned int *>(pData);
	for (unsigned int i = numBytes >> 2; i--; pData32++) {  // Reverse 4 bytes at a time in parallel
		*pData32 = ((*pData32 & 0xaaaaaaaa) >> 1) | ((*pData32 & 0x55555555) << 1);
		*pData32 = ((*pData32 & 0xcccccccc) >> 2) | ((*pData32 & 0x33333333) << 2);
		*pData32 = ((*pData32 & 0xf0f0f0f0) >> 4) | ((*pData32 & 0x0f0f0f0f) << 4);
	}
	unsigned char *pData8 = reinterpret_cast<unsigned char *>(pData32);
	for (unsigned int i = numBytes & 3; i--; pData8++)
		*pData8 = static_cast<unsigned char>(((*pData8 * 0x0802ul & 0x22110ul) | (*pData8 * 0x8020ul & 0x88440ul)) * 0x10101ul >> 16);
}

//-----------------------------------------------------------------------------------------------------------
void ByteSwapBuf16(void *pData, unsigned int count) {
	unsigned short *pData16 = reinterpret_cast<unsigned short *>(pData);
	for (unsigned int i = 0; i < count; i++)
		ByteSwap16(pData16[i]);
}

//-----------------------------------------------------------------------------------------------------------
void ByteSwapBuf32(void *pData, unsigned int count) {
	unsigned int *pData32 = reinterpret_cast<unsigned int *>(pData);
	for (unsigned int i = 0; i < count; i++)
		ByteSwap32(pData32[i]);
}

//-----------------------------------------------------------------------------------------------------------
static const int FS_PATH_MAX_LEN = 2048;

#ifdef _WIN32

static std::wstring utf8to16(const std::string &utf8) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> utf16conv;
	std::wstring utf16 = utf16conv.from_bytes(utf8.data());
	return std::move(utf16);
}

static std::string utf16to8(const std::wstring &utf16) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> utf16conv;
	std::string utf8 = utf16conv.to_bytes(utf16.data());
	return std::move(utf8);
}

enum FsAccessType {
	FS_ACCESS_EXISTS = 0,
	FS_ACCESS_WRITEONLY = 2,
	FS_ACCESS_READONLY = 4,
	FS_ACCESS_READWRITE = 6
};

#endif

//-----------------------------------------------------------------------------------------------------------
bool FsCreateDir(const std::string &fullPathDir) {
#ifdef _WIN32
	std::wstring utf16 = utf8to16(fullPathDir);
	if (_waccess(utf16.c_str(), FS_ACCESS_EXISTS) == 0) {
#else
	struct stat data;
	if (stat(fullPathDir.c_str(), &data) == 0) {
#endif
		size_t index = fullPathDir.find_last_of("/\\");
		if (index == fullPathDir.length() - 1)
			index = fullPathDir.find_last_of("/\\", index - 1);
		if (index != std::string::npos && !FsCreateDir(std::move(fullPathDir.substr(0, index))))
			return false;
#ifdef _WIN32
		if (_wmkdir(utf16.c_str()) != 0)
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
	std::wstring utf16 = utf8to16(fullPathDir);
	_wsystem((L"if exist " + utf16 + L" rmdir /s /q " + utf16).c_str());
	return true;
	//return RemoveDirectoryW(utf8to16(fullPath).c_str()) != 0;
#else
	auto funcDel = [](const char *fullPath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) -> int {
		int rv = remove(fullPath);
		if (rv)
			perror(fullPath);
		return rv;
	};
	return nftw(fullPathDir.c_str(), funcDel, 64, FTW_DEPTH | FTW_PHYS);
#endif
}

//-----------------------------------------------------------------------------------------------------------
bool FsDeleteFile(const std::string &fullPathFile) {
#ifdef _WIN32
	return DeleteFileW(utf8to16(fullPathFile).c_str()) != 0;
#else
	return remove(fullPathFile.c_str()) != -1;
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
	struct _stat data;
	return _wstat(utf8to16(fullPath).c_str(), &data) == 0 && (data.st_mode & _S_IFDIR);
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
	std::wstring utf16 = utf8to16(fullPath);
	return _waccess(utf16.c_str(), FS_ACCESS_EXISTS) == 0 && _waccess(utf16.c_str(), FS_ACCESS_WRITEONLY) == -1;
#else
	struct stat data;
	return stat(fullPath.c_str(), &data) == 0 && access(fullPath.c_str(), W_OK) == -1;
#endif
}

//-----------------------------------------------------------------------------------------------------------
bool FsIsHidden(const std::string &fullPath) {
#ifdef _WIN32
	DWORD attr = GetFileAttributesW(utf8to16(fullPath).c_str());
	return !!(attr & FILE_ATTRIBUTE_HIDDEN);
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
	struct __stat64 data;
	_wstat64(utf8to16(fullPath).c_str(), &data);
#else
	struct stat data;
	stat(fullPath.c_str(), &data);
#endif
	return data.st_atime;
}

//-----------------------------------------------------------------------------------------------------------
time_t FsGetTimeMod(const std::string &fullPath) {
#ifdef _WIN32
	struct __stat64 data;
	_wstat64(utf8to16(fullPath).c_str(), &data);
#else
	struct stat data;
	stat(fullPath.c_str(), &data);
#endif
	return data.st_mtime;
}

//-----------------------------------------------------------------------------------------------------------
time_t FsGetTimeCreate(const std::string &fullPath) {
#ifdef _WIN32
	struct __stat64 data;
	_wstat64(utf8to16(fullPath).c_str(), &data);
#else
	struct stat data;
	stat(fullPath.c_str(), &data);
#endif
	return data.st_ctime;
}

//-----------------------------------------------------------------------------------------------------------
std::string FsGetTempDir(bool includeSlash) {
#ifdef _WIN32
	wchar_t path[FS_PATH_MAX_LEN];
	GetTempPathW(FS_PATH_MAX_LEN, path);
	return utf16to8(path) + (includeSlash ? "\\" : "");
#else
	const char *path = getenv("TMPDIR");
	if (path == 0)
		path = "/tmp";
	return std::string(path) + (includeSlash ? "/" : "");
#endif
}

//-----------------------------------------------------------------------------------------------------------
std::string FsGetWorkingDir(bool includeSlash) {
#ifdef _WIN32
	wchar_t path[FS_PATH_MAX_LEN];
	GetCurrentDirectoryW(FS_PATH_MAX_LEN, path);
	return utf16to8(path) + (includeSlash ? "\\" : "");
#else
	char path[FS_PATH_MAX_LEN];
	getcwd(path, FS_PATH_MAX_LEN);
	return std::string(path) + (includeSlash ? "/" : "");
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
	return utf16to8(path);
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
std::string FsGetFullPath(const std::string &path) {
#ifdef _WIN32
	wchar_t out16[FS_PATH_MAX_LEN];
	_wfullpath(out16, utf8to16(path).c_str(), FS_PATH_MAX_LEN - 1);
	return utf16to8(out16);
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
	struct _stat64 data;
	_wstat64(utf8to16(fullPathFile).c_str(), &data);
#else
	struct stat64 data;
	stat64(fullPathFile.c_str(), &data);
#endif
	return data.st_size;
}

}  // namespace

#endif  // SAW_IO_IMPLEMENTATION
