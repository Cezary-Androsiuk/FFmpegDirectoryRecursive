#include "FFExecute.hpp"

int FFExecute::m_performedFFmpegs = 0;
int FFExecute::m_correctlyPerformedFFmpegs = 0;
int FFExecute::m_failedFFmpegs = 0;
int FFExecute::m_skippedFFmpegs = 0;
int FFExecute::m_totalFFmpegsToPerform = 0;
SkipAction FFExecute::m_skipAction = SkipAction::Skip;

std::ofstream FFExecute::m_ffOFile;
str FFExecute::m_ffOFileName;
fs::path FFExecute::m_ffOFileDirectory;
str FFExecute::m_strDuration;

void FFExecute::handleOutput(cstr line)
{
    FFExecute::addTextToFFOFile(line);
    FFExecute::printOutputToCMD(line);
}

void FFExecute::printOutputToCMD(cstr line)
{
    constexpr int strtimeTextSize = 11;

    constexpr const char timeText[] = "kB time="; // kb are for better match
    size_t timeTextPos = line.find(timeText);
    if(timeTextPos == str::npos)
        return;

    str strtime = line.substr(timeTextPos + sizeof(timeText), strtimeTextSize);
    int timePassed = FFExecute::getInterpretationOfTime(strtime);
    FFExecute::printProgress(timePassed);
}

long long FFExecute::myStoll(cstr string) noexcept
{
    // std::stoll throws exception while argument is an empty string...
    long long integer = 0;

    try{
        integer = std::stoll(string);
    } catch(...) {}

    return integer;
}

size_t FFExecute::getInterpretationOfTime(cstr strtime)
{
    if(strtime.empty()) // i thought, that std::stoll will return 0 when gets empty string ...
        return 0;

    // well coded by chat gpt
    std::stringstream ss(strtime);
    std::string segment;
    size_t hours = 0, minutes = 0, seconds = 0, milliseconds = 0;
    
    std::getline(ss, segment, ':');
    hours = FFExecute::myStoll(segment);
    
    std::getline(ss, segment, ':');
    minutes = FFExecute::myStoll(segment);
    
    std::getline(ss, segment, '.');
    seconds = FFExecute::myStoll(segment);
    
    std::getline(ss, segment);
    milliseconds = FFExecute::myStoll(segment);
    
    return (hours * 3600 * 1000) + 
        (minutes * 60 * 1000) + 
        (seconds * 1000) + 
        milliseconds;
    
}

str FFExecute::getCurrentTime()
{
    time_t rawTime;
    time(&rawTime);
    tm *time = localtime(&rawTime);

    char buffer[16];
    sprintf(buffer, "%04d%02d%02d_%02d%02d%02d", 
        time->tm_year+1900, time->tm_mon+1, time->tm_mday,
        time->tm_hour, time->tm_min, time->tm_sec);
    
    return str(buffer);
}

str FFExecute::changeOutputFileNameIfNeeded(cstr file)
{
    fs::path parentPath = fs::path(file).parent_path();
    str filename = fs::path(file).filename().string();
    int dotPos = filename.find_last_of('.');
    str rawFilename = filename.substr(0, dotPos);
    str rawExtension = filename.substr(dotPos+1);

    fs::path newFileName = parentPath / (rawFilename + "." + rawExtension);
    int index = 0;  
    while(fs::exists(newFileName))
    {
        printf("      out file with filename '%s' " COLOR_YELLOW "already exist" COLOR_RESET "!\n", newFileName.filename().string().c_str());
        newFileName = parentPath / (rawFilename + "_" + std::to_string(index++) + "." + rawExtension);
    }
    return newFileName.string();
}

void FFExecute::openFFOFile()
{
    if(m_ffOFileName.empty())
        m_ffOFileName = FFMPEG_OUTPUT_FILE(FFExecute::getCurrentTime());

    m_ffOFile = std::ofstream((m_ffOFileDirectory / m_ffOFileName), std::ios::app);
    if(!m_ffOFile.good())
    {
        // i won't fuck with that here...
        // will be handled in addTextToFFOFile
        fprintf(stderr, "Error while oppening ffmpeg output file!");
    }
}

void FFExecute::addTextToFFOFile(cstr ffmpegOutput)
{
    if(!m_ffOFile.good())
        printf("ffmpeg output file failed, output text: %s", ffmpegOutput.c_str());
    else
        m_ffOFile << ffmpegOutput;
}

void FFExecute::closeFFOFile()
{
    m_ffOFile << "\n";
    m_ffOFile.close();
}

int FFExecute::lengthOfNumber(int number)
{
    return std::to_string(number).size();
}

str FFExecute::numberThatOccupiesGivenSpace(int number, int space)
{
    str strNumber = FFExecute::splitNumberByThousands(number);
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

void FFExecute::clearLine(int len)
{
    // clear line (windows os only)
    printf("\r"); // move to start
    printf("%s", std::string(len, ' ').c_str());
    printf("\r"); // move to start
}

str FFExecute::splitNumberByThousands(int number, char separator)
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

void FFExecute::printProgress(int progress)
{
    // create format __23/0123, or _123/0123 ...
    FFExecute::clearLine(15+2 + m_strDuration.size() * 2 + 4);
    str strProgress = FFExecute::numberThatOccupiesGivenSpace(progress, m_strDuration.size());
    printf("    progress:  %s / %s", strProgress.c_str(), m_strDuration.c_str());

    fflush(stdout);
}

void FFExecute::skipFileAction()
{
    fprintf(stderr, "    inFile is already H265! " COLOR_YELLOW "Skipping" COLOR_RESET "!\n");
    FFExecute::addTextToFFOFile("    inFile is already H265! Skipping!\n");
}
bool FFExecute::copyFileAction(cstr from, cstr to)
{
    fprintf(stderr, "    inFile is already H265! " COLOR_YELLOW "Copying" COLOR_RESET "!\n");
    FFExecute::addTextToFFOFile("    inFile is already H265! Copying!\n");

    // copy file
    try
    {
        fs::copy_file(fs::path(from), fs::path(to));
        return true;
    }
    catch(std::filesystem::filesystem_error &e)
    {
        fprintf(stderr, "    Copying file " COLOR_RED "failed" COLOR_RESET ": %s\n", e.what());
        FFExecute::addTextToFFOFile("    Copying file failed: " + str( e.what() ));
        ++ m_failedFFmpegs;
        return false;
    }
}
bool FFExecute::moveFileAction(cstr from, cstr to)
{
    fprintf(stderr, "    inFile is already H265! " COLOR_YELLOW "Moving" COLOR_RESET "!\n");
    FFExecute::addTextToFFOFile("    inFile is already H265! Moving!\n");

    // move file
    try
    {
        fs::rename(fs::path(from), fs::path(to));
        return true;
    }
    catch(std::filesystem::filesystem_error &e)
    {
        fprintf(stderr, "    Moving file " COLOR_RED "failed" COLOR_RESET ": %s\n", e.what());
        FFExecute::addTextToFFOFile("    Moving file failed: " + str( e.what() ));
        ++ m_failedFFmpegs;
        return false;
    }
}

void FFExecute::moveCorrectlyFinishedFile(cstr from, cstr to)
{
    try
    {
        fs::rename(fs::path(from), fs::path(to));
    }
    catch(std::filesystem::filesystem_error &e)
    {
        fprintf(stderr, "    Moving finished file " COLOR_RED "failed" COLOR_RESET ": %s\n", e.what());
        FFExecute::addTextToFFOFile("    Moving finished file failed: " + str( e.what() ));
        OtherError::addError("Moving finished file from: '" + from + "', to: '" + to + "' failed", __PRETTY_FUNCTION__);
    }
}

void FFExecute::handleAlreadyH265File(cstr inFile, str outFile)
{
    // in file is already H265 format!

    // possible actins here:
    // 1: skip inFile as it is
    // 2: copy inFile to outFile    
    // 3: move inFile to outFile  

    switch (m_skipAction)
    {
    case SkipAction::Skip:
        FFExecute::skipFileAction();
        ++ m_skippedFFmpegs;
        break;
        
    case SkipAction::Copy:
        if(FFExecute::copyFileAction(inFile, outFile))
        { // file copied
            ++ m_skippedFFmpegs;
        }
        else // copying file failed
        {
            ++ m_failedFFmpegs;
        }
        break;
        
    case SkipAction::Move:
        if(FFExecute::moveFileAction(inFile, outFile))
        { // file moved
            ++ m_skippedFFmpegs;
        }
        else // moving file failed
        {
            ++ m_failedFFmpegs;
        }
        break;
    }

    return;
}

void FFExecute::runFFmpegTest(cstr inFile)
{
    FFExecute::openFFOFile();

    printf("Statuts: %s", FFExecute::makeFileProgressPostfix().c_str());
    FFExecute::runFFmpegTest2(inFile);
    ++ m_performedFFmpegs;
    if(m_performedFFmpegs == m_totalFFmpegsToPerform)
        printf("Statuts: %s\n\n", FFExecute::makeFileProgressPostfix().c_str());

    FFExecute::addTextToFFOFile("    Status " + FFExecute::makeFileProgressPostfix(false));
    FFExecute::addTextToFFOFile(" -------------------------------------------------------------------");
    FFExecute::addTextToFFOFile("-------------------------------------------------------------------\n\n\n\n\n\n\n\n\n");
    
    FFExecute::closeFFOFile();
}

void FFExecute::runFFmpegForce(cstr inFile, cstr outFile, cstr moveFile)
{
    FFExecute::openFFOFile();

    FFExecute::runFFmpegForce2(inFile, outFile, moveFile);
    ++ m_performedFFmpegs;
    
    printf("    Statuts: %s\n\n", FFExecute::makeFileProgressPostfix().c_str());
    FFExecute::addTextToFFOFile("    Status " + FFExecute::makeFileProgressPostfix(false));
    FFExecute::addTextToFFOFile(" -------------------------------------------------------------------");
    FFExecute::addTextToFFOFile("-------------------------------------------------------------------\n\n\n\n\n\n\n\n\n");
    
    FFExecute::closeFFOFile();
}

void FFExecute::runFFmpegStandard(cstr inFile, cstr outFile, cstr moveFile)
{
    FFExecute::openFFOFile();

    FFExecute::runFFmpegStandard2(inFile, outFile, moveFile);
    ++ m_performedFFmpegs;
    
    printf("    Statuts: %s\n\n", FFExecute::makeFileProgressPostfix().c_str());
    FFExecute::addTextToFFOFile("    Status " + FFExecute::makeFileProgressPostfix(false));
    FFExecute::addTextToFFOFile(" -------------------------------------------------------------------");
    FFExecute::addTextToFFOFile("-------------------------------------------------------------------\n\n\n\n\n\n\n\n\n");
    
    FFExecute::closeFFOFile();
}

void FFExecute::runFFmpegTest2(cstr inFile)
{
    printf("  in:  %s\n", inFile.c_str());    FFExecute::addTextToFFOFile("    in:  " + inFile + "\n");
    
    FFExecute::addTextToFFOFile("  Starting new ffmpeg\n");
    FFExecute::addTextToFFOFile("    in:  " + inFile + "\n");
    // seprate case when input file not exist
    if(!fs::exists(inFile))
    {
        fprintf(stderr, "    " COLOR_RED "Input file not exist" COLOR_RESET "!\n");
        FFExecute::addTextToFFOFile("    Input file not exist!\n");
        ++ m_failedFFmpegs;
        return;
    }

    FFExecute::addTextToFFOFile("\n    FFprobe output:\n");
    FFTester::setHandleFFprobeOutput(FFExecute::addTextToFFOFile);

    if(!FFTester::canBeConvertedToH265(inFile, false))
    {
        if(FFTester::errorOccur())
        {
            fprintf(stderr, "    error occur while checking if file is H265: %s\n", 
                FFTester::getErrorInfo().c_str());
            FFExecute::addTextToFFOFile("    error occur while checking if file is H265: " + 
                FFTester::getErrorInfo() + "\n");
            ++ m_failedFFmpegs;
            return;
        }

        ++ m_skippedFFmpegs;
        return;
    }
    ++ m_correctlyPerformedFFmpegs;
}

void FFExecute::runFFmpegForce2(cstr inFile, str outFile, cstr moveFile)
{
    printf("  Starting new FFmpeg\n");          FFExecute::addTextToFFOFile("  Starting new ffmpeg\n");

    // check if out file exist (case when in input dir are exist files 1.mp4 and 1.mkv)
    outFile = FFExecute::changeOutputFileNameIfNeeded(outFile);

    printf("    in:  %s\n", inFile.c_str());    FFExecute::addTextToFFOFile("    in:  " + inFile + "\n");
    printf("    out: %s\n", outFile.c_str());   FFExecute::addTextToFFOFile("    out: " + outFile + "\n");

    // seprate case when input file not exist
    if(!fs::exists(inFile))
    {
        fprintf(stderr, "    " COLOR_RED "Input file not exist" COLOR_RESET "!\n");
        FFExecute::addTextToFFOFile("    Input file not exist!\n");
        ++ m_failedFFmpegs;
        return;
    }

    FFExecute::addTextToFFOFile("\n    FFprobe output:\n");
    FFTester::setHandleFFprobeOutput(FFExecute::addTextToFFOFile);

    // FFTester will set "strDuration" so there is no way to get rid of it
    if(!FFTester::canBeConvertedToH265(inFile))
    {
        if(FFTester::errorOccur())
        {
            fprintf(stderr, "    error occur while executing FFTester: %s\n", 
                FFTester::getErrorInfo().c_str());
            FFExecute::addTextToFFOFile("    error occur while executing FFTester: " + 
                FFTester::getErrorInfo() + "\n");
            ++ m_failedFFmpegs;
            return;
        }
    }

    str command = "ffmpeg -i \"" + inFile + "\" -c:v libx265 -vtag hvc1 \"" + outFile + "\"";
    command += " 2>&1"; // move stderr to stdout (connect them)

    FFExecute::addTextToFFOFile("\n    FFmpeg output:\n");
    FFExecute::addTextToFFOFile("    command: " + command + "\n\n");

    int duration = FFExecute::getInterpretationOfTime(FFTester::getStrDuration());
    m_strDuration = FFExecute::splitNumberByThousands(duration);
    FFExecute::printProgress(0);


    
    FILE* pipe = pipeOpen(command.c_str(), "r");
    if (!pipe) {
        fprintf(stderr, "    " COLOR_RED "Cannot open the pipe" COLOR_RESET "!\n");
        FFExecute::addTextToFFOFile("    Cannot open the pipe!\n");
        return;
    }

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
        try
        {
            FFExecute::handleOutput(str(buffer));
        }
        catch(const std::exception& e)
        {
            printf("error while handling output\n");
        }
        
    }

    int ffmpegExitCode = pipeClose(pipe);


    
    if(ffmpegExitCode) // not equal 0 - error occur in ffmpeg
    {
        ++ m_failedFFmpegs;
        printf("\n");
        fprintf(stderr, "    FFmpeg " COLOR_RESET COLOR_RED "failed" COLOR_RESET " with code %d!\n", ffmpegExitCode);
        FFExecute::addTextToFFOFile("    FFmpeg failed with code " + std::to_string(ffmpegExitCode) + "!\n");
    }
    else // no error - ffmpeg finished correctly
    {
        ++ m_correctlyPerformedFFmpegs;
        FFExecute::printProgress(duration);
        printf("\n");
        fprintf(stderr, "    FFmpeg " COLOR_GREEN "finished" COLOR_RESET "!\n");
        FFExecute::addTextToFFOFile("    FFmpeg finished!\n");

        // move finished files to directory, that contains finished files
        FFExecute::moveCorrectlyFinishedFile(inFile, moveFile);
    }
}

void FFExecute::runFFmpegStandard2(cstr inFile, str outFile, cstr moveFile)
{
    printf("  Starting new FFmpeg\n");          FFExecute::addTextToFFOFile("  Starting new ffmpeg\n");

    // check if out file exist (case when in input dir are exist files 1.mp4 and 1.mkv)
    outFile = FFExecute::changeOutputFileNameIfNeeded(outFile);
    
    printf("    in:  %s\n", inFile.c_str());    FFExecute::addTextToFFOFile("    in:  " + inFile + "\n");
    printf("    out: %s\n", outFile.c_str());   FFExecute::addTextToFFOFile("    out: " + outFile + "\n");

    // seprate case when input file not exist
    if(!fs::exists(inFile))
    {
        fprintf(stderr, "    " COLOR_RED "Input file not exist" COLOR_RESET "!\n");
        FFExecute::addTextToFFOFile("    Input file not exist!\n");
        ++ m_failedFFmpegs;
        return;
    }

    FFExecute::addTextToFFOFile("\n    FFprobe output:\n");
    FFTester::setHandleFFprobeOutput(FFExecute::addTextToFFOFile);

    if(!FFTester::canBeConvertedToH265(inFile))
    {
        if(FFTester::errorOccur())
        {
            fprintf(stderr, "    error occur while checking if file is H265: %s\n", 
                FFTester::getErrorInfo().c_str());
            FFExecute::addTextToFFOFile("    error occur while checking if file is H265: " + 
                FFTester::getErrorInfo() + "\n");
            ++ m_failedFFmpegs;
            return;
        }

        FFExecute::handleAlreadyH265File(inFile, outFile);
        return;
    }

    str command = "ffmpeg -i \"" + inFile + "\" -c:v libx265 -vtag hvc1 \"" + outFile + "\"";
    command += " 2>&1"; // move stderr to stdout (connect them)

    FFExecute::addTextToFFOFile("\n\n\n\n\n\n    FFmpeg output:\n");
    FFExecute::addTextToFFOFile("    command: " + command + "\n\n");

    int duration = FFExecute::getInterpretationOfTime(FFTester::getStrDuration());
    m_strDuration = FFExecute::splitNumberByThousands(duration);
    FFExecute::printProgress(0);


    
    FILE* pipe = pipeOpen(command.c_str(), "r");
    if (!pipe) {
        fprintf(stderr, "    " COLOR_RED "Cannot open the pipe" COLOR_RESET "!\n");
        FFExecute::addTextToFFOFile("    Cannot open the pipe!\n");
        return;
    }

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
        try
        {
            FFExecute::handleOutput(str(buffer));
        }
        catch(const std::exception& e)
        {
            printf("error while handling output\n");
        }
        
    }

    int ffmpegExitCode = pipeClose(pipe);


    
    if(ffmpegExitCode) // not equal 0 - error occur in ffmpeg
    {
        ++ m_failedFFmpegs;
        printf("\n");
        fprintf(stderr, "    FFmpeg " COLOR_RESET COLOR_RED "failed" COLOR_RESET " with code %d!\n", ffmpegExitCode);
        FFExecute::addTextToFFOFile("    FFmpeg failed with code " + std::to_string(ffmpegExitCode) + "!\n");
    }
    else // no error - ffmpeg finished correctly
    {
        ++ m_correctlyPerformedFFmpegs;
        FFExecute::printProgress(duration);
        printf("\n");
        fprintf(stderr, "    FFmpeg " COLOR_GREEN "finished" COLOR_RESET "!\n");
        FFExecute::addTextToFFOFile("    FFmpeg finished!\n");

        // move finished files to directory, that contains finished files
        FFExecute::moveCorrectlyFinishedFile(inFile, moveFile);
    }
}

str FFExecute::makeFileProgressPostfix(bool addColors)
{
    // total_ffmpegs_to_perform should be the largest number
    int lengthOfCount = FFExecute::lengthOfNumber(m_totalFFmpegsToPerform);
    str output = "[ ";

    output += addColors ? COLOR_WHITE : "";
    output += FFExecute::numberThatOccupiesGivenSpace( m_correctlyPerformedFFmpegs, lengthOfCount );
    output += addColors ? COLOR_RESET : "";
    output += "/";
    
    output += addColors ? COLOR_WHITE : "";
    output += FFExecute::numberThatOccupiesGivenSpace( m_performedFFmpegs, lengthOfCount );
    output += addColors ? COLOR_RESET : "";
    output += "/";
    
    output += addColors ? COLOR_WHITE : "";
    output += FFExecute::numberThatOccupiesGivenSpace( m_totalFFmpegsToPerform, lengthOfCount );
    output += addColors ? COLOR_RESET : "";
    output += " ";
    
    output += addColors ? COLOR_RED : "";
    output += FFExecute::numberThatOccupiesGivenSpace( m_failedFFmpegs, lengthOfCount );
    output += addColors ? COLOR_RESET : "";
    
    if(m_skipAction != SkipAction::Force)
    {
        output += "/";
        output += addColors ? COLOR_YELLOW : "";
        output += FFExecute::numberThatOccupiesGivenSpace( m_skippedFFmpegs, lengthOfCount );
        output += addColors ? COLOR_RESET : "";
    }

    output += " ]";

    return output;
}

void FFExecute::setTotalFFmpegsToPerform(int count)
{
    m_totalFFmpegsToPerform = count;
}

void FFExecute::setSkipAction(SkipAction skipAction)
{
    m_skipAction = skipAction;
}

void FFExecute::setffOFileDirectory(fs::path directory)
{
    m_ffOFileDirectory = directory;
}

void FFExecute::runFFmpeg(cstr inFile, cstr outFile, cstr moveFile)
{
    if(m_skipAction == SkipAction::Test)
        FFExecute::runFFmpegTest(inFile);
    else if(m_skipAction == SkipAction::Force)
        FFExecute::runFFmpegForce(inFile, outFile, moveFile);
    else
        FFExecute::runFFmpegStandard(inFile, outFile, moveFile);
}