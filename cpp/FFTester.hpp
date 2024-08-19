#ifndef FFTESTER_HPP
#define FFTESTER_HPP

#include <cstdio>
#include <string>
// #include <sstream> // for getInterpretationOfTime
// #include <ctime>
// #include <fstream>

typedef std::string str;
typedef const std::string &cstr;

#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define TEXT_BOLD     "\033[1m"

#define pipeOpen(...) _popen(__VA_ARGS__)
#define pipeClose(...) _pclose(__VA_ARGS__)

class FFTester
{
    // if ffprobe output contains fragment, then algorithm will assume that is the encoding
    // i find out, that for example videoPrefix can occur only once
    struct Patterns{
        static const str durationPrefix;
        static const int strtimeTextSize;
        static const str videoPrefix;
        static const str patternH265;
        static const str patternH264;
    };

    static void handleOutput(cstr line);

    enum VerificationStatus{
        InVerification = 0,
        IsH265,
        IsH264,
        IsOther
    };

    static VerificationStatus m_verificationStatus;
    static str m_errorInfo;
    static str m_strDuration;

public:
    static bool canBeConvertedToH265(cstr filePath); // returns true if file is h265

    static cstr getErrorInfo();
    static cstr getStrDuration();
    static bool errorOccur();

};

#endif