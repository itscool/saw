// saw_fs.h - Cross platform file system manipulation
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
// - v1.00 - 03/17/16 - Initial release by Scott Williams

//-----------------------------------------------------------------------------------------------------------
// Notes

//-----------------------------------------------------------------------------------------------------------
// Todo
// @@ Better error reporting (besides true/false)
// @@ Hand-written code for Windows version of FsDeleteDir so we can handle errors
// @@ FsGetRelPath(fullPathFrom, fullPathTo)
// @@ FsEnum(fullPath)
// @@ FsWatch(fullPath)

#ifndef _SAW_FS_H_INCLUDED
#define _SAW_FS_H_INCLUDED

namespace saw {

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

}  // namespace

#endif  // _SAW_FS_H_INCLUDED

#ifdef SAW_FS_IMPLEMENTATION

#include <string>
#include <sys/types.h>      // This has to preceed sys/stat.h
#include <sys/stat.h>
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

const int FS_PATH_MAX_LEN = 2048;

#ifdef _WIN32

std::wstring utf8to16(const std::string &utf8) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> utf16conv;
	std::wstring utf16 = utf16conv.from_bytes(utf8.data());
	return std::move(utf16);
}

std::string utf16to8(const std::wstring &utf16) {
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

#endif  // SAW_FS_IMPLEMENTATION
