#include "FFExecute.hpp"

int FFExecute::m_performedFFmpegs = 0;
int FFExecute::m_correctlyPerformedFFmpegs = 0;
int FFExecute::m_failedFFmpegs = 0;
int FFExecute::m_skippedFFmpegs = 0;
int FFExecute::m_totalFFmpegsToPerform = 0;
SkipAction FFExecute::m_skipAction = SkipAction::Skip;

std::ofstream FFExecute::m_ffOFile;
str FFExecute::m_ffOFileName;
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

size_t FFExecute::getInterpretationOfTime(cstr strtime)
{
    if(strtime.empty()) // i thought, that std::stoll will return 0 when gets empty string ...
        return 0;

    // well coded by chat gpt
    std::stringstream ss(strtime);
    std::string segment;
    size_t hours = 0, minutes = 0, seconds = 0, milliseconds = 0;

    std::getline(ss, segment, ':');
    hours = std::stoll(segment);
    
    std::getline(ss, segment, ':');
    minutes = std::stoll(segment);
    
    std::getline(ss, segment, '.');
    seconds = std::stoll(segment);
    std::getline(ss, segment);
    milliseconds = std::stoll(segment);

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
        time->tm_year, time->tm_mon, time->tm_mday,
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

    m_ffOFile = std::ofstream(m_ffOFileName, std::ios::app);
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
    // create format __23/0123, or _123/0123
    FFExecute::clearLine(15+2 + m_strDuration.size() * 2);

    str strProgress = FFExecute::splitNumberByThousands(progress);
    str space = str(m_strDuration.size() - strProgress.size(), ' ');

    printf("    progress:  %s%s / %s", space.c_str(), strProgress.c_str(), m_strDuration.c_str());
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

void FFExecute::_runFFmpeg(cstr inFile, str outFile)
{
    printf("  Starting new FFmpeg\n");
    FFExecute::addTextToFFOFile("  Starting new ffmpeg\n");

    // check if out file exist (case when in input dir are exist files 1.mp4 and 1.mkv)
    outFile = FFExecute::changeOutputFileNameIfNeeded(outFile);

    printf("    in:  %s\n", inFile.c_str());    FFExecute::addTextToFFOFile("    in:  " + inFile + "\n");
    printf("    out: %s\n", outFile.c_str());   FFExecute::addTextToFFOFile("    out: " + outFile + "\n");

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
        // in file is already H265 format!

        // possible actins here:
        // 1: skip inFile as it is
        // 2: copy inFile to outFile    
        // 3: move inFile to outFile  

        switch (m_skipAction)
        {
        case SkipAction::Skip:
            FFExecute::skipFileAction();
            break;
            
        case SkipAction::Copy:
            if(!FFExecute::copyFileAction(inFile, outFile))
                return;
            break;
            
        case SkipAction::Move:
            if(!FFExecute::moveFileAction(inFile, outFile))
                return;
            break;
        }

        ++ m_skippedFFmpegs;
        return;
    }


    str command = "ffmpeg -i \"" + inFile + "\" -c:v libx265 -vtag hvc1 \"" + outFile + "\"";
    command += " 2>&1"; // move stderr to stdout (connect them)

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
        FFExecute::handleOutput(str(buffer));

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
    }
}

str FFExecute::makeFileProgressPostfix(bool addColors)
{
    // total_ffmpegs_to_perform should be the largest number
    int lengthOfCount = FFExecute::lengthOfNumber(m_totalFFmpegsToPerform);
    str cpFFSpace = str(lengthOfCount - FFExecute::lengthOfNumber(m_correctlyPerformedFFmpegs), ' ');
    str pFFSpace = str(lengthOfCount - FFExecute::lengthOfNumber(m_performedFFmpegs), ' ');
    str tFFtpSpace = str(lengthOfCount - FFExecute::lengthOfNumber(m_totalFFmpegsToPerform), ' ');
    str fFFSpace = str(lengthOfCount - FFExecute::lengthOfNumber(m_failedFFmpegs), ' ');
    str sFFSpace = str(lengthOfCount - FFExecute::lengthOfNumber(m_skippedFFmpegs), ' ');
    str cpFF = cpFFSpace + std::to_string(m_correctlyPerformedFFmpegs);
    str pFF = pFFSpace +  std::to_string(m_performedFFmpegs);
    str tFFtp = tFFtpSpace +  std::to_string(m_totalFFmpegsToPerform);
    str fFF = fFFSpace +  std::to_string(m_failedFFmpegs);
    str sFF = sFFSpace +  std::to_string(m_skippedFFmpegs);

    // correctly_performed_ffmpegs / performed_ffmpegs / total_ffmpegs_to_perform   failed_ffmpegs / skipped_ffmpegs
    if(addColors)
        return COLOR_WHITE/*grey color*/ + cpFF + "/" + pFF + "/" + tFFtp + " " COLOR_RESET COLOR_RED + fFF + COLOR_RESET "/" COLOR_YELLOW + sFF + COLOR_RESET;
    else 
        return cpFF + "/" + pFF + "/" + tFFtp + " " + fFF + "/" + sFF;
}

void FFExecute::setTotalFFmpegsToPerform(int count)
{
    m_totalFFmpegsToPerform = count;
}

void FFExecute::setSkipAction(SkipAction skipAction)
{
    m_skipAction = skipAction;
}

void FFExecute::runFFmpeg(cstr inFile, cstr outFile)
{
    FFExecute::openFFOFile();
    FFExecute::_runFFmpeg(inFile, outFile);
    ++ m_performedFFmpegs;
    
    printf("    Statuts: [ %s ]\n\n", FFExecute::makeFileProgressPostfix().c_str());
    FFExecute::addTextToFFOFile("    Status [ " + FFExecute::makeFileProgressPostfix(false) + " ]\n\n\n\n\n\n\n\n\n");
    
    FFExecute::closeFFOFile();
}