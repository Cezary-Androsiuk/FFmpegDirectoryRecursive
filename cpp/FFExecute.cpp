#include "FFExecute.hpp"

int FFExecute::m_performedFFmpegs = 0;
int FFExecute::m_correctlyPerformedFFmpegs = 0;
int FFExecute::m_failedFFmpegs = 0;
int FFExecute::m_skippedFFmpegs = 0;
int FFExecute::m_totalFFmpegsToPerform = 0;
SkipAction FFExecute::m_skipAction = SkipAction::Skip;
int FFExecute::m_lastExecuteStatus = 0;

fs::path FFExecute::changeOutputFileNameIfNeeded(fs::path path)
{
    const fs::path parentPath = path.parent_path();
    const std::wstring stem = path.stem().wstring();
    const std::wstring extension = path.extension().wstring();

    int index = 0;
    while(fs::exists(path))
    {
        printf("      out file with filename '%ls' " COLOR_YELLOW "already exist" COLOR_RESET "!\n", path.filename().wstring().c_str());
        HandlePipeOutput::addToFFOFile("out file with filename '" + path.filename().string() + "' already exist!\n");

        wchar_t indexText[32];
        swprintf(indexText, L"_%06d", index++);
        
        path = parentPath / (stem + std::wstring(indexText) +  extension);
    }
    
    if(index)
    {
        printf("      using '%ls' as output filename instead!\n", path.filename().wstring().c_str());
        HandlePipeOutput::addToFFOFile("using '" + path.filename().string() + "' as output filename instead!\n");
    }
    return path;
}



void FFExecute::skipFileAction()
{
    fprintf(stderr, "    inFile is already H265! " COLOR_YELLOW "Skipping" COLOR_RESET "!\n");
    HandlePipeOutput::addToFFOFile("    inFile is already H265! Skipping!\n");
}
bool FFExecute::copyFileAction(cpath from, cpath to)
{
    fprintf(stderr, "    inFile is already H265! " COLOR_YELLOW "Copying" COLOR_RESET "!\n");
    HandlePipeOutput::addToFFOFile("    inFile is already H265! Copying!\n");

    // copy file
    try
    {
        fs::copy_file(fs::path(from), fs::path(to));
        return true;
    }
    catch(std::filesystem::filesystem_error &e)
    {
        fprintf(stderr, "    Copying file " COLOR_RED "failed" COLOR_RESET ": %s\n", e.what());
        HandlePipeOutput::addToFFOFile("    Copying file failed: " + str( e.what() ));
        ++ m_failedFFmpegs;
        return false;
    }
}
bool FFExecute::moveFileAction(cpath from, cpath to)
{
    fprintf(stderr, "    inFile is already H265! " COLOR_YELLOW "Moving" COLOR_RESET "!\n");
    HandlePipeOutput::addToFFOFile("    inFile is already H265! Moving!\n");

    // move file
    try
    {
        fs::rename(fs::path(from), fs::path(to));
        return true;
    }
    catch(std::filesystem::filesystem_error &e)
    {
        fprintf(stderr, "    Moving file " COLOR_RED "failed" COLOR_RESET ": %s\n", e.what());
        HandlePipeOutput::addToFFOFile("    Moving file failed: " + str( e.what() ));
        ++ m_failedFFmpegs;
        return false;
    }
}

void FFExecute::handleAlreadyH265File(cpath inFile, cpath outFile)
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



void FFExecute::handleStop(cpath inFile, cpath outFile)
{
    // rename back if i decided to use currently unsused TemporaryRename.cpp
    if(fs::exists(outFile))
    {
        try
        {
            fs::remove(outFile);
        }
        catch(const std::exception& e)
        {
            fprintf(stderr, COLOR_RED "error while removing outputFile" COLOR_RESET ": %ls, what() = %s\n",
                outFile.wstring().c_str(), e.what());
            HandlePipeOutput::addToFFOFile("error while removing outputFile: "+outFile.string()+", what() = " + e.what());
        }
        
    }
    printf("performing ffmpeg was stopped, due to Ctrl+C combination was pressed\n");
    HandlePipeOutput::addToFFOFile("\n\nperforming ffmpeg was stopped, due to Ctrl+C combination was pressed\n");
}

void FFExecute::moveDateOfFile(cpath from, cpath to)
{
    ChangeFileDate::setHandleFFprobeOutput(HandlePipeOutput::addToFFOFile);

    if(!ChangeFileDate::fromFileToFile(from, to))
    {
        fprintf(stderr, "    Changing date of the file " COLOR_RED "failed" COLOR_RESET "\n");
        HandlePipeOutput::addToFFOFile("    Changing date of the file failed");
        OtherError::addError(L"Changing date of the file failed", __PRETTY_FUNCTION__);
    }
}

void FFExecute::moveCorrectlyFinishedFile(cpath from, cpath to)
{
    if( !fs::exists(from) )
    {
        fprintf(stderr, "    " COLOR_RED "Moving finished file failed" COLOR_RESET "! Source file not exist '%s'\n", from.string().c_str());
        HandlePipeOutput::addToFFOFile("    Moving finished file failed! Source file not exist '" + from.string() + "'");
        OtherError::addError(L"Moving finished file failed - Source file not exist '" + from.wstring() + L"'", __PRETTY_FUNCTION__);
        return;
    }
    
    if( fs::exists(to) )
    {
        fprintf(stderr, "    " COLOR_RED "Moving finished file failed" COLOR_RESET "! Output file already exist '%s'\n", to.string().c_str());
        HandlePipeOutput::addToFFOFile("    Moving finished file failed! Output file already exist '" + to.string() + "'");
        OtherError::addError(L"Moving finished file failed - Output file already exist '" + to.wstring() + L"'", __PRETTY_FUNCTION__);
        return;
    }

    try
    {
        fs::rename(from, to);
    }
    catch(std::filesystem::filesystem_error &e)
    {
        fprintf(stderr, "    Moving finished file " COLOR_RED "failed" COLOR_RESET ": %s\n", e.what());
        HandlePipeOutput::addToFFOFile("    Moving finished file failed: " + str( e.what() ));
        OtherError::addError(L"Moving finished file from: '" + from.wstring() + L"', to: '" + to.wstring() + L"' failed", __PRETTY_FUNCTION__);
    }
}



void FFExecute::runFFmpegTest(cpath inFile)
{
    printf("Status: %s", FFExecute::makeFileProgressPostfix().c_str());
    FFExecute::runFFmpegTest2(inFile);
    if(WinConsoleHandler::combinationCtrlCPressed()) return; // just exist if pressed Ctrl+C

    ++ m_performedFFmpegs;
    if(m_performedFFmpegs == m_totalFFmpegsToPerform)
        printf("\nStatus: %s\n\n", FFExecute::makeFileProgressPostfix().c_str());

    HandlePipeOutput::addToFFOFile("\n    Status " + FFExecute::makeFileProgressPostfix(false) + "\n");
}

void FFExecute::runFFmpegStandard(fs::path inFile, fs::path outFile, fs::path moveFile)
{
    printf("  Starting new FFmpeg\n");          HandlePipeOutput::addToFFOFile("  Starting new ffmpeg\n");

    // check if out file exist (case when in input dir are exist files 1.mp4 and 1.mkv)
    fs::path validOutFile = FFExecute::changeOutputFileNameIfNeeded(outFile);

    printf("    in:  %s\n", inFile.string().c_str());    
    printf("    out: %s\n", validOutFile.string().c_str());   
    printf("    move: %s\n", moveFile.string().c_str());   
    HandlePipeOutput::addToFFOFile(
        "    in:  " + inFile.string() + "\n"
        "    out: " + validOutFile.string() + "\n"
        "    move: " + moveFile.string() + "\n");

    // if(!TemporaryRename::makeNameSimple(inFile, validOutFile, moveFile))
    // {
    //     printf(COLOR_RED "making name simpler failed" COLOR_RESET "!\n");
    //     HandlePipeOutput::addToFFOFile(L"Making name simpler failed in '"+ inFile.wstring() +L"'!\n");
    //     ++ m_failedFFmpegs;
    //     m_lastExecuteStatus = 1;
    //     return;
    // }
    // HandlePipeOutput::addTextToFFOFile(TemporaryRename::getRenameInfo());

    // printf("    inTmp:  %ls\n", inFile.wstring().c_str());    HandlePipeOutput::addToFFOFile(L"    inTmp:  " + inFile.wstring() + L"\n");
    // printf("    outTmp: %ls\n", validOutFile.wstring().c_str());   HandlePipeOutput::addToFFOFile(L"    outTmp: " + validOutFile.wstring() + L"\n");
    // printf("    moveTmp: %ls\n", moveFile.wstring().c_str());   HandlePipeOutput::addToFFOFile(L"    moveTmp: " + moveFile.wstring() + L"\n");

    FFExecute::runFFmpegStandard2(inFile, validOutFile, moveFile);

    // if(!TemporaryRename::restoreName(inFile, validOutFile, moveFile))
    // {
    //     printf(COLOR_RED "restoring original names failed" COLOR_RESET "!\n");
    //     ADD_OTHER_ERROR(L"restoring original name failed for " + inFile.wstring());
    // }

    if(WinConsoleHandler::combinationCtrlCPressed()) {
        FFExecute::handleStop(inFile, validOutFile);
        // just exist if pressed Ctrl+C
        return;
    }

    ++ m_performedFFmpegs;
    
    printf("    Status: %s\n\n", FFExecute::makeFileProgressPostfix().c_str());
    HandlePipeOutput::addToFFOFile("\n    Status " + FFExecute::makeFileProgressPostfix(false));
}

void FFExecute::runFFmpegTest2(cpath inFile)
{
    str inFileStr = inFile.string();
    printf("  in:  %s\n", inFileStr.c_str());    HandlePipeOutput::addToFFOFile("    in:  " + inFileStr + "\n");
    
    HandlePipeOutput::addToFFOFile("  Starting new ffmpeg\n");
    HandlePipeOutput::addToFFOFile("    in:  " + inFileStr + "\n");
    
    // seprate case when input file not exist
    if(!fs::exists(inFile))
    {
        fprintf(stderr, "    " COLOR_RED "Input file not exist" COLOR_RESET "!\n");
        HandlePipeOutput::addToFFOFile("    Input file not exist!\n");
        ++ m_failedFFmpegs;
        m_lastExecuteStatus = 1;
        return;
    }

    HandlePipeOutput::addToFFOFile("\n    TestPipe output:\n");
    TestPipe::setHandleDirOutput(HandlePipeOutput::addToFFOFile);
    if(!TestPipe::testName(inFile))
    {
        fprintf(stderr, "    " COLOR_RED "Input file cannot be passed as argument to other program" COLOR_RESET "!\n");
        HandlePipeOutput::addToFFOFile("    Input file cannot be passed as argument to other program!\n");
        ++ m_failedFFmpegs;
        m_lastExecuteStatus = 1;
        return;
    }

    HandlePipeOutput::addToFFOFile("\n    FFprobe output:\n");
    FFTester::setHandleFFprobeOutput(HandlePipeOutput::addToFFOFile);

    if(!FFTester::canBeConvertedToH265(inFile, false))
    {
        if(FFTester::errorOccur())
        {
            fprintf(stderr, "    error occur while checking if file is H265: %s\n", 
                FFTester::getErrorInfo().c_str());
            HandlePipeOutput::addToFFOFile("    error occur while checking if file is H265: " + 
                FFTester::getErrorInfo() + "\n");
            ++ m_failedFFmpegs;
            m_lastExecuteStatus = 1;
            return;
        }

        ++ m_skippedFFmpegs;
        m_lastExecuteStatus = 2;
        return;
    }
    ++ m_correctlyPerformedFFmpegs;
    m_lastExecuteStatus = 0;
}

void FFExecute::runFFmpegStandard2(cpath inFile, cpath outFile, cpath moveFile)
{
    // seprate case when input file not exist
    if(!fs::exists(inFile))
    {
        fprintf(stderr, "    " COLOR_RED "Input file not exist" COLOR_RESET "!\n");
        HandlePipeOutput::addToFFOFile("    Input file not exist!\n");
        ++ m_failedFFmpegs;
        m_lastExecuteStatus = 1;
        return;
    }

    HandlePipeOutput::addToFFOFile("\n\n\n\n\n\n    TestPipe output:\n");
    TestPipe::setHandleDirOutput(HandlePipeOutput::addToFFOFile);
    if(!TestPipe::testName(inFile))
    {
        fprintf(stderr, "    " COLOR_RED "Input file cannot be passed as argument to other program" COLOR_RESET "!\n");
        HandlePipeOutput::addToFFOFile("    Input file cannot be passed as argument to other program!\n");
        ++ m_failedFFmpegs;
        m_lastExecuteStatus = 1;
        return;
    }

    HandlePipeOutput::addToFFOFile("\n\n\n\n\n\n    FFprobe output:\n");
    FFTester::setHandleFFprobeOutput(HandlePipeOutput::addToFFOFile);

    if(!FFTester::canBeConvertedToH265(inFile))
    {
        if(FFTester::errorOccur())
        {
            if(m_skipAction == SkipAction::Force)
            {
                fprintf(stderr, "    error occur while executing FFTester: %s\n", 
                    FFTester::getErrorInfo().c_str());
                HandlePipeOutput::addToFFOFile("    error occur while executing FFTester: " + 
                    FFTester::getErrorInfo() + "\n");
            }
            else
            {
                fprintf(stderr, "    error occur while executing FFTester, unable to check"
                    " if file is H265: %s\n", FFTester::getErrorInfo().c_str());
                HandlePipeOutput::addToFFOFile("    error occur while checking if file "
                    "is H265: " + FFTester::getErrorInfo() + "\n");
            }
            ++ m_failedFFmpegs;
            m_lastExecuteStatus = 1;
            return;
        }

        if(m_skipAction != SkipAction::Force)
        {
            FFExecute::handleAlreadyH265File(inFile, outFile); // m_skippedFFmpegs are there
            m_lastExecuteStatus = 1;
            return;
        }
    }

    std::wstring command = L"ffmpeg -i \"" + inFile.wstring() + 
        L"\" -c:v libx265 -vtag hvc1 \"" + outFile.wstring() + L"\"";
    command += L" 2>&1"; // move stderr to stdout (connect them)

    HandlePipeOutput::addToFFOFile(
        "\n\n\n\n\n\n    FFmpeg output:\n"
        "    command: " + str(command.begin(), command.end()) + "\n\n");

    int duration = HandlePipeOutput::getInterpretationOfTime(FFTester::getStrDuration());
    HandlePipeOutput::setStringDuration(HandlePipeOutput::splitNumberByThousands(duration, ' '));
    HandlePipeOutput::printProgress(0, HandlePipeOutput::splitNumberByThousands(duration, ' '));


    
    FILE* pipe = wpipeOpen(command.c_str(), L"r");
    if (!pipe) {
        fprintf(stderr, "    " COLOR_RED "Cannot open the pipe" COLOR_RESET "!\n");
        HandlePipeOutput::addToFFOFile("    Cannot open the pipe!\n");
        ++ m_failedFFmpegs;
        m_lastExecuteStatus = 1;
        return;
    }

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
        if(WinConsoleHandler::combinationCtrlCPressed())
        {
            // no exit status, all stops
            return;
        }
        
        try
        {
            HandlePipeOutput::handleOutput(str(buffer));
        }
        catch(const std::exception& e)
        {
            printf("error while handling output in FFExecute\n");
        }
    }

    int ffmpegExitCode = pipeClose(pipe);


    
    if(ffmpegExitCode) // not equal 0 - error occur in ffmpeg
    {
        ++ m_failedFFmpegs;
        m_lastExecuteStatus = 1;
        printf("\n");
        fprintf(stderr, "    FFmpeg " COLOR_RESET COLOR_RED "failed" COLOR_RESET " with code %d!\n", ffmpegExitCode);
        HandlePipeOutput::addToFFOFile("    FFmpeg failed with code " + std::to_string(ffmpegExitCode) + "!\n");
    }
    else // no error - ffmpeg finished correctly
    {
        ++ m_correctlyPerformedFFmpegs;
        m_lastExecuteStatus = 0;
        HandlePipeOutput::printProgress(duration, HandlePipeOutput::splitNumberByThousands(duration, ' '));
        printf("\n");
        fprintf(stderr, "    FFmpeg " COLOR_GREEN "finished" COLOR_RESET "!\n");
        HandlePipeOutput::addToFFOFile("    FFmpeg finished!\n");

        // change create/update date of compressed file
        FFExecute::moveDateOfFile(inFile, outFile);
        
        // move finished files to directory, that contains finished files
        FFExecute::moveCorrectlyFinishedFile(inFile, moveFile);
    }
}

str FFExecute::makeFileProgressPostfix(bool addColors)
{
    // total_ffmpegs_to_perform should be the largest number
    int lengthOfCount = std::to_string(m_totalFFmpegsToPerform).size();
    str output = "[ ";

    output += addColors ? COLOR_WHITE : "";
    output += HandlePipeOutput::numberThatOccupiesGivenSpace( m_correctlyPerformedFFmpegs, lengthOfCount );
    output += addColors ? COLOR_RESET : "";
    output += "/";
    
    output += addColors ? COLOR_WHITE : "";
    output += HandlePipeOutput::numberThatOccupiesGivenSpace( m_performedFFmpegs, lengthOfCount );
    output += addColors ? COLOR_RESET : "";
    output += "/";
    
    output += addColors ? COLOR_WHITE : "";
    output += HandlePipeOutput::numberThatOccupiesGivenSpace( m_totalFFmpegsToPerform, lengthOfCount );
    output += addColors ? COLOR_RESET : "";
    output += " ";
    
    output += addColors ? COLOR_RED : "";
    output += HandlePipeOutput::numberThatOccupiesGivenSpace( m_failedFFmpegs, lengthOfCount );
    output += addColors ? COLOR_RESET : "";
    
    if(m_skipAction != SkipAction::Force)
    {
        output += "/";
        output += addColors ? COLOR_YELLOW : "";
        output += HandlePipeOutput::numberThatOccupiesGivenSpace( m_skippedFFmpegs, lengthOfCount );
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

void FFExecute::runFFmpeg(cpath inFile, cpath outFile, cpath moveFile)
{
    HandlePipeOutput::openFFOFile();

    try{
        if(m_skipAction == SkipAction::Test)
            FFExecute::runFFmpegTest(inFile);
        else
            FFExecute::runFFmpegStandard(inFile, outFile, moveFile);

        HandlePipeOutput::addToFFOFile(
            "\n -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --"
            "-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --"
            "-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --"
            "-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --"
            "-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --"
            "-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --"
            "-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --"
            "-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --"
            "-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --"
            "-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --"
            "-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --"
            "\n\n\n\n\n\n\n\n\n");
    }
    catch(std::exception &e)
    {
        fprintf(stderr, COLOR_RED "SOMETHING WENT TERRIBLY WRONG ! %s" COLOR_RESET "\n", e.what());
    }
    
    HandlePipeOutput::closeFFOFile();

    
        
    if(WinConsoleHandler::combinationCtrlCPressed()) // do not clean file if Ctrl+C pressed
        return;
    HandlePipeOutput::cleanFFOFile();
}

int FFExecute::getLastExecuteStatus()
{
    return m_lastExecuteStatus;
}

int FFExecute::getCorrectlyPerformedFFmpegs()
{
    return m_correctlyPerformedFFmpegs;
}