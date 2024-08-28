#ifndef MAIN_METHODS_HPP
#define MAIN_METHODS_HPP

#include <cstdio>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm> // stringTolower
#include <cctype> // stringTolower

#include <unistd.h>
#include <fcntl.h>

#include "enums/SkipAction.hpp"

namespace fs = std::filesystem;
typedef std::string str;
typedef std::wstring wstr;
typedef const str &cstr;
typedef const wstr &cwstr;
typedef std::vector<str> vstr;
typedef const fs::path &cpath;
typedef std::vector<fs::path> vpath;


extern str lastError;
extern const char possibleSeparators[];


#define FLEXIBLE_ARGUMENTS false

#define FUNC_START {lastError = "";}

#define DEFAULT_PATH fs::current_path()
#define DEFAULT_EXTENSIONS {"mkv", "mp4"}
#define DEFAULT_SKIP_ACTION SkipAction::Skip

#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_WHITE   "\033[37m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define TEXT_BOLD     "\033[1m"


vstr splitStringByChar(cstr str, char separator);
vstr splitExtensionsInput(str input);
void stringTolower(str &string);
SkipAction handleInputSkipAction(str input);
bool handleArgs(int argc, const char **argv, void *arguments[]);
bool argsValidConst(int argc, const char **argv, fs::path *const directory, vstr *const extensions, SkipAction *const skipAction);
bool argsValidFlexible(int argc, const char **argv, fs::path *const directory, vstr *const extensions, SkipAction *const skipAction);

bool isDirectoryEmpty(fs::path directory);
bool createDirectoryIfValid(fs::path outDirectory);
bool copyStructureOfFolders(fs::path sourceDir, fs::path targetDir);

fs::path createOutputDirectory(cpath inputDirectory, bool removeDirIfExist = false);
fs::path createOCFDirectory(cpath inputDirectory, bool removeDirIfExist = false); // OCFDirectory is OutputCompletedFilesDirectory

void printStatusInfo(SkipAction skipAction);

fs::path createOutputFile(cpath inFile, cpath inDirectory, cpath outDirectory);
fs::path createOFCFile(cpath inFile, cpath inDirectory, cpath OFCDirectory);

void deleteDirectoryIfEmpty(fs::path outDirectory);
bool rm_all(const fs::path& path);



#endif // MAIN_METHODS_HPP