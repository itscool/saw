// saw_io.h - Io abstraction including file and memory implementations
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
// - v1.00 - 03/12/16 - Initial release by Scott Williams

//-----------------------------------------------------------------------------------------------------------
// Notes

//-----------------------------------------------------------------------------------------------------------
// Todo

#ifndef _SAW_IO_H_INCLUDED
#define _SAW_IO_H_INCLUDED

#include <stddef.h>
#include <stdarg.h>
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

bool IoOpenFile(Io *io, const char *filename, IoAccessType access);
bool IoOpenMem(Io *io, const void *pMem, size_t size);
bool IoOpenVec(Io *io, std::vector<char> *pVec);
inline void IoClose(const Io *io);

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

}  // namespace

#endif  // _SAW_IO_H_INCLUDED

#ifdef SAW_IO_IMPLEMENTATION

#include <memory.h>
#ifdef _WIN32
// Needed for proper utf8 support in windows
#	include <codecvt>
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
	case IO_ACCESS_RW: io->handle = fopen(filename, "r+b"); if (!io->handle) io->handle = fopen(filename.c_str(), "w+b"); break;
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

}  // namespace

#endif  // SAW_IO_IMPLEMENTATION
