#include "FFTester.hpp"
const str FFTester::Patterns::durationPrefix = "Duration: ";
const int FFTester::Patterns::strtimeTextSize = 11;
const str FFTester::Patterns::videoPrefix = "Video: ";
const str FFTester::Patterns::patternH265 = "Video: hevc";
const str FFTester::Patterns::patternH264 = "Video: h264";
void (*FFTester::m_addTextToFFOFile)(cstr) = nullptr;

FFTester::VerificationStatus FFTester::m_verificationStatus;
str FFTester::m_errorInfo;
str FFTester::m_strDuration;

void FFTester::handleOutput(cstr line)
{
    if(m_addTextToFFOFile != nullptr)
        m_addTextToFFOFile(line);

    if(m_verificationStatus != VerificationStatus::InVerification)
        return;

    if(m_strDuration.empty())
    {
        size_t durationPrefixPos = line.find(FFTester::Patterns::durationPrefix);
        if(durationPrefixPos != str::npos)
        {
            size_t durationPos = durationPrefixPos + FFTester::Patterns::durationPrefix.size();
            // example line: "  Duration: 00:00:14.77, start: 0.000000, bitrate: 373 kb/s"
            m_strDuration = line.substr(durationPos, FFTester::Patterns::strtimeTextSize); // gives "00:00:14.77"
        }
    }

    if(line.find(Patterns::videoPrefix) != str::npos) 
    {
        // found video line 
        if(line.find(Patterns::patternH265) != str::npos)
        {
            // found h265
            m_verificationStatus = VerificationStatus::IsH265;
        } 
        if(line.find(Patterns::patternH264) != str::npos)
        {
            // found h264
            m_verificationStatus = VerificationStatus::IsH264;
        }
        else
        {
            // is other one
            m_verificationStatus == VerificationStatus::IsOther;
        }
    }
}

bool FFTester::canBeConvertedToH265(cpath filePath, bool verbose)
{
    m_errorInfo.clear();
    m_strDuration.clear();
    m_verificationStatus = VerificationStatus::InVerification;

    std::wstring command = L"ffprobe -i \"" + filePath.wstring() + L"\"";
    command += L" 2>&1"; // move stderr to stdout (connect them)

    if(m_addTextToFFOFile != nullptr)
        m_addTextToFFOFile("    command: " + str(command.begin(), command.end()) + "\n\n");

    FILE* pipe = wpipeOpen(command.c_str(), L"r");
    if (!pipe) {
        fprintf(stderr, "Cannot open the pipe!\n");
        m_errorInfo = "Cannot open the pipe!";
        return false;
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
        try
        {
            FFTester::handleOutput(str(buffer));
        }
        catch(const std::exception& e)
        {
            printf("error while handling output in FFTester\n");
        }
    }

    int ffprobeExitCode = pipeClose(pipe);
    
    if(ffprobeExitCode) // error occur
    {
        m_errorInfo = "FFprobe failed with code: " + std::to_string(ffprobeExitCode) + "!";
        fprintf(stderr, "      FFprobe " COLOR_RED "failed" COLOR_RESET " with code %d!\n", ffprobeExitCode);
        return false;
    }
    
    if(verbose)
    {
        if(m_verificationStatus == VerificationStatus::IsH265)
            printf("      FFTester found that video encoding is H265\n");
        else if(m_verificationStatus == VerificationStatus::IsH265)
            printf("      FFTester found that video encoding is H264\n");
        else if(m_verificationStatus == VerificationStatus::IsOther)
            printf("      FFTester found that video encoding is other than H265\n");
    }

    return m_verificationStatus != VerificationStatus::IsH265;
}

void FFTester::setHandleFFprobeOutput(void (*func)(cstr))
{
    m_addTextToFFOFile = func;
}

cstr FFTester::getErrorInfo()
{
    return m_errorInfo;
}

cstr FFTester::getStrDuration()
{
    return m_strDuration;
}

bool FFTester::errorOccur()
{
    return !m_errorInfo.empty();
}
