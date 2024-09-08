#include "HandlePipeOutput.hpp"

std::string HandlePipeOutput::m_stringDuration;
std::wofstream HandlePipeOutput::m_ffOFile;
fs::path HandlePipeOutput::m_ffOFileDirectory;
fs::path HandlePipeOutput::m_ffOFilePath;
bool HandlePipeOutput::m_ffOFileIsOpen = false;


long long HandlePipeOutput::myStoll(cstr string) noexcept
{
    // std::stoll throws exception while argument is an empty string...
    long long integer = 0;

    try{
        integer = std::stoll(string);
    } catch(...) {}

    return integer;
}

size_t HandlePipeOutput::getInterpretationOfTime(cstr stringTime)
{
    if(stringTime.empty()) // i thought, that std::stoll will return 0 when gets empty string ...
        return 0;

    // well coded by chat gpt
    std::stringstream ss(stringTime);
    std::string segment;
    size_t hours = 0, minutes = 0, seconds = 0, milliseconds = 0;
    
    std::getline(ss, segment, ':');
    hours = HandlePipeOutput::myStoll(segment);
    
    std::getline(ss, segment, ':');
    minutes = HandlePipeOutput::myStoll(segment);
    
    std::getline(ss, segment, '.');
    seconds = HandlePipeOutput::myStoll(segment);
    
    std::getline(ss, segment);
    milliseconds = HandlePipeOutput::myStoll(segment);
    
    return (hours * 3600 * 1000) + 
        (minutes * 60 * 1000) + 
        (seconds * 1000) + 
        milliseconds;
}

void HandlePipeOutput::clearLine(int len)
{
    // clear line (windows os only)
    printf("\r"); // move to start
    printf("%s", std::string(len, ' ').c_str());
    printf("\r"); // move to start
}

str HandlePipeOutput::splitNumberByThousands(int number, char separator)
{
    str strNumber = std::to_string(number);
    str splited;
    while(strNumber.size() > 3)
    {
        str part = strNumber.substr(strNumber.size() - 3, 3);
        strNumber.erase(strNumber.size() - 3, 3);

        splited.insert(0, part);
        
        if(!strNumber.empty())
            splited.insert(0, 1, separator);
    };
    splited.insert(0, strNumber);

    return splited;
}

str HandlePipeOutput::numberThatOccupiesGivenSpace(int number, int space)
{
    str strNumber = HandlePipeOutput::splitNumberByThousands(number, ' ');
    int occupiedSpace = strNumber.size();
    int additionalSpace = space - occupiedSpace;
    if(additionalSpace > 0)
    {
        return str(additionalSpace, ' ') + strNumber;
    }
    else 
    {
        return strNumber;
    }
}

str HandlePipeOutput::getCurrentTime()
{
    time_t rawTime;
    time(&rawTime);
    tm *time = localtime(&rawTime);

    char buffer[32];
    sprintf(buffer, "%04d%02d%02d_%02d%02d%02d", 
        time->tm_year+1900, time->tm_mon+1, time->tm_mday,
        time->tm_hour, time->tm_min, time->tm_sec);
    
    return str(buffer);
}

void HandlePipeOutput::createFFOFilePath()
{
    str fileName = "FFmpegRec_output-" + HandlePipeOutput::getCurrentTime() + ".txt";
    if(m_ffOFileDirectory.empty())
    {
        printf("HandlePipeOutput::m_ffOFileDirectory was not set!\n");
        exit(1);
    }
    m_ffOFilePath = m_ffOFileDirectory / fileName;
}



void HandlePipeOutput::printOutputToCMD(cstr line)
{
    constexpr int strtimeTextSize = 11;

    constexpr const char timeText[] = "kB time="; // kb are for better match
    size_t timeTextPos = line.find(timeText);
    if(timeTextPos == str::npos)
        return;

    str strtime = line.substr(timeTextPos + sizeof(timeText), strtimeTextSize);
    int timePassed = HandlePipeOutput::getInterpretationOfTime(strtime);
    HandlePipeOutput::printProgress(timePassed, m_stringDuration);
}


void HandlePipeOutput::addTextToFFOFile(cwstr line)
{
    if(!m_ffOFile.good())
        printf("ffmpeg output file failed, output text: %ls", line.c_str());
    else
        m_ffOFile << line;
}



void HandlePipeOutput::printProgress(int progress, cstr duration)
{
    // create format __23/0123, or _123/0123 ...
    HandlePipeOutput::clearLine(15+2 + duration.size() * 2 + 4);
    str strProgress = HandlePipeOutput::numberThatOccupiesGivenSpace(progress, duration.size());
    printf("    progress:  %s / %s", strProgress.c_str(), duration.c_str());

    fflush(stdout);
}

void HandlePipeOutput::handleOutput(cstr line)
{
    HandlePipeOutput::printOutputToCMD(line);

    HandlePipeOutput::addToFFOFile(line);
}

void HandlePipeOutput::addToFFOFile(cstr text)
{
    wstr wstrText;
    for(char c : text)
        wstrText.push_back( static_cast<wchar_t>(c) );

    HandlePipeOutput::addTextToFFOFile(wstrText);
}

void HandlePipeOutput::addToFFOFile(cwstr text)
{
    HandlePipeOutput::addTextToFFOFile(text);
}



void HandlePipeOutput::openFFOFile()
{
    if(m_ffOFileIsOpen)
    {
        printf("FFOFile is already open!\n");
        return;
    }

    // if file was not oppened yet
    if(m_ffOFilePath.empty())
        HandlePipeOutput::createFFOFilePath();

    m_ffOFile = std::wofstream(m_ffOFilePath, std::ios::app);
    if(!m_ffOFile.good())
    {
        // i won't fuck with that here...
        // will be handled in addTextToFFOFile
        fprintf(stderr, "Error while oppening ffmpeg output file!");
    }
    m_ffOFileIsOpen = true;
}

void HandlePipeOutput::closeFFOFile()
{
    if(!m_ffOFileIsOpen)
    {
        printf("FFOFile is already closed!\n");
        return;
    }
    m_ffOFile << L"\n";
    m_ffOFile.close();
    m_ffOFileIsOpen = false;
}

fs::path HandlePipeOutput::moveFFOFileToTemporary()
{
    str stem = m_ffOFilePath.stem().string();
    str ext = m_ffOFilePath.extension().string();

    fs::path tempPath;

    do{
        str tempFilename = stem + "-" + HandlePipeOutput::getCurrentTime() + ext;
        tempPath = m_ffOFileDirectory / tempFilename;
    }
    while(fs::exists(tempPath));

    return tempPath;
}

bool HandlePipeOutput::lineIsSpam(cwstr line)
{
    // spam line example: 
    // frame=   11 fps=0.0 q=34.0 size=       0kB time=00:00:02.08 bitrate=   0.2kbits/s dup=2 drop=0 speed=3.86x    
    // there can be multiple of them and but mostly they just occupies the space
    if(line.find(L"frame=") != 0)
        return false;

    if(line.find(L"size=") == str::npos)
        return false;
        
    if(line.find(L"time=") == str::npos)
        return false;

    return true;
}

void HandlePipeOutput::cleanFFOFile()
{
    bool ffOFileClosedHere = false;
    if(m_ffOFileIsOpen)
    {
        // close previously oppened file
        HandlePipeOutput::closeFFOFile();
        ffOFileClosedHere = true;
    }

    fs::path tempFFOfile;

    std::wifstream ifile(m_ffOFilePath);
    if(!ifile.good())
    {
        fprintf(stderr, "error while oppening FFOFile! Can't clean it\n");
        return;
    }
    std::wofstream ofile(tempFFOfile);
    if(!ifile.good())
    {
        fprintf(stderr, "error while oppening tempFFOfile! Can't clean it\n");
        return;
    }

    wstr buffer;
    bool lastWasSpam = false;
    while(std::getline(ifile, buffer))
    {
        if(HandlePipeOutput::lineIsSpam(buffer))
        {
            ofile << L"<>";
            lastWasSpam = true;
            continue;
        }

        if(lastWasSpam)
            ofile << std::endl;

        ofile << buffer << std::endl;
        lastWasSpam = false;
    }

    ifile.close();
    ofile.close();

    try
    {
        fs::rename(tempFFOfile, m_ffOFilePath);
    }
    catch(const std::exception& e)
    {
        fprintf(stderr, "Error while renaming tempFFOfile(%ls) to m_ffOFilePath(%ls) : %s\n",
            tempFFOfile.wstring().c_str(), m_ffOFilePath.wstring().c_str(), e.what());
    }
    

    if(ffOFileClosedHere)
    {
        HandlePipeOutput::openFFOFile();
        if(!m_ffOFileIsOpen)
        {
            fprintf(stderr, "error while reoppening FFOFile!\n");
            return;
        }
    }
}



void HandlePipeOutput::setFFOFileDirectory(cpath ffOFileDirectory)
{
    m_ffOFileDirectory = ffOFileDirectory;
}

void HandlePipeOutput::setStringDuration(cstr stringDuration)
{
    m_stringDuration = stringDuration;
}