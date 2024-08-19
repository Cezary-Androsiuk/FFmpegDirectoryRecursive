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
typedef const std::string &cstr;
typedef std::vector<str> vstr;
typedef const fs::path &cpath;
typedef std::vector<fs::path> vpath;


extern str lastError;
extern const char possibleSeparators[];

#define FUNC_START {lastError = "";}

#define DEFAULT_PATH fs::current_path()
#define DEFAULT_EXTENSIONS {"mkv", "mp4"}
#define DEFAULT_SKIP_ACTION SkipAction::Skip

#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_WHITE   "\033[37m"


vstr splitStringByChar(cstr str, char separator);
vstr splitExtensionsInput(str input);
void stringTolower(str &string);
SkipAction handleInputSkipAction(str input);
bool argsValid(int argc, const char **argv, fs::path *const directory, vstr *const extensions, SkipAction *const skipAction);
bool argsValidFlexible(int argc, const char **argv, fs::path *const directory, vstr *const extensions, SkipAction *const skipAction);

bool isDirectoryEmpty(fs::path directory);
bool createOutputDirectory(fs::path outDirectory);
fs::path createOutputFilename(cpath inFile, cpath outDirectory);

void deleteDirectoryIfEmpty(fs::path outDirectory);
bool rm_all(const fs::path& path);



#endif // MAIN_METHODS_HPP