#ifndef TEMPORARY_RENAME_HPP
#define TEMPORARY_RENAME_HPP

#include <cstdio>
#include <string>
#include <chrono>
#include <random>
#include <filesystem>

#include "OtherError.hpp"
#include "BetterConversion.hpp"

namespace fs = std::filesystem;

typedef const fs::path &cpath;
typedef std::string str;
typedef const std::string &cstr;
typedef std::wstring wstr;

#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"

class TemporaryRename
{
    static str getCurrentTime();
    static str makeRandomFilename(str extension);
    static fs::path createValidAlternativeName(cpath path);
    static bool tryRename(cpath from, cstr fromName, cpath to, cstr toName, bool otherError = false);

public: 
    static bool makeNameSimple(fs::path &inFile, fs::path &outFile, fs::path &moveFile);
    static bool restoreName(fs::path &inFile, fs::path &outFile, fs::path &moveFile);
    static wstr getRenameInfo();

private:
    static bool m_temporaryRenamed;
    static fs::path m_inFileOld;
    static fs::path m_inFileTmp;
    static fs::path m_outFileOld;
    static fs::path m_outFileTmp;
    static fs::path m_moveFileOld;
    static fs::path m_moveFileTmp;

};

#endif // TEMPORARY_RENAME_HPP