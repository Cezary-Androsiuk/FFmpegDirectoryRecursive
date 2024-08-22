#include <cstdio>
#include <string>
#include <vector>
#include <filesystem>

#include "cpp/MainMethods.hpp"
#include "cpp/FFExecute.hpp"
#include "cpp/ListMaker.hpp"
#include "cpp/enums/SkipAction.hpp"

// locate files recursive

// compile:
// g++ main.cpp cpp\MainMethods.cpp cpp\FFExecute.cpp cpp\FFTester.cpp cpp\ListMaker.cpp -o ffmpegRec.exe

// instalation(add ffmpegRec.exe to PATH environment):
// 1 in windows search type "Edit the system environment variables" 
// 2 press "Environment Variables..." button
// 3 in bottom part (System variables) find variable named "Path" and double click on it
// 4 press on the right site the "New" button and type path to directory, where executable file (created after compilation) is located
// in example path to this executable (for now) is "D:\vscode\c++\projects\FFmpegDirectoryRecursive"
// now, you can open cmd in any directory and just in command prompt type "ffmpegRec . mkv+mp4"

#define IN_DEBUG true

int main(int argc, const char **argv)
{
    printf("\n");

    fs::path directory;
    vstr extensions;
    SkipAction skipAction;
    void* arguments[] = {&directory, &extensions, &skipAction};
    if( !handleArgs(argc, argv, arguments) )
    {
        // messages are handle in 'handleArgs' function
        return 1;
    }

    printf("Selected directory: %s\n", directory.string().c_str());
    
    fs::path outDirectory = createOutputDirectory(directory, IN_DEBUG);
    if(outDirectory == fs::path())
    {
        // messages are handle in 'createOutputDirectory' function
        return 1;
    }

    fs::path OFCDirectory = createOCFDirectory(directory, IN_DEBUG);
    if(OFCDirectory == fs::path())
    {
        // messages are handle in 'createOCFDirectory' function
        return 1;
    }

    printf("Found files:\n");
    vpath listOfFiles = ListMaker::listOfFiles(directory, extensions); // listOfFiles prints them

    FFExecute::setTotalFFmpegsToPerform(listOfFiles.size());
    FFExecute::setSkipAction(skipAction);
    FFExecute::setffOFileDirectory(directory);
    FFExecute::setOFCDirectory(OFCDirectory);
    
    str skippedText;
    if(skipAction == SkipAction::Skip) skippedText = "skipped";
    else if(skipAction == SkipAction::Copy) skippedText = "copied";
    else if(skipAction == SkipAction::Move) skippedText = "moved";

    printf("\n[ " COLOR_WHITE "correctlyPerformed / performed / totalToPerform   " 
        COLOR_RESET COLOR_RED "failed" COLOR_RESET " / " COLOR_YELLOW "%s" COLOR_RESET " ]\n", skippedText.c_str());

    str filesProgress = FFExecute::makeFileProgressPostfix();
    printf("Status: [ %s ]\n\n", filesProgress.c_str()); 
    for(const auto &inFilePath : listOfFiles)
    {
        // all files in list are valid at this point
        fs::path outFilePath = createOutputFilename(inFilePath, directory, outDirectory);

        std::wstring inFileWString = inFilePath.wstring();
        std::string inFileString(inFileWString.begin(), inFileWString.end());
        std::wstring outFileWString = outFilePath.wstring();
        std::string outFileString(outFileWString.begin(), outFileWString.end());

        FFExecute::runFFmpeg(inFileString, outFileString);
    }


    deleteDirectoryIfEmpty(outDirectory);

    printf("Finished all FFmpegs!\n");
}