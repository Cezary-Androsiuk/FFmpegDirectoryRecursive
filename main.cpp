#include <cstdio>
#include <string>
#include <vector>
#include <filesystem>

#include "cpp/OtherError.hpp"
#include "cpp/MainMethods.hpp"
#include "cpp/FFExecute.hpp"
#include "cpp/ListMaker.hpp"
#include "cpp/enums/SkipAction.hpp"

// locate files recursive

// compile:
// g++ main.cpp cpp\MainMethods.cpp cpp\FFExecute.cpp cpp\FFTester.cpp cpp\ListMaker.cpp cpp\OtherError.cpp -o ffmpegRec.exe

// instalation(add ffmpegRec.exe to PATH environment):
// 1 in windows search type "Edit the system environment variables" 
// 2 press "Environment Variables..." button
// 3 in bottom part (System variables) find variable named "Path" and double click on it
// 4 press on the right site the "New" button and type path to directory, where executable file (created after compilation) is located
// in example path to this executable (for now) is "D:\vscode\c++\projects\FFmpegDirectoryRecursive(github)"
// now, you can open cmd in any directory and just in command prompt type "ffmpegRec . mkv+mp4 move"

#define IN_DEBUG false

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
    
    printStatusInfo(skipAction);
    if(skipAction != SkipAction::Test)
    {
        str filesProgress = FFExecute::makeFileProgressPostfix();
        printf("Status: %s\n\n", filesProgress.c_str()); 
    }

    for(const auto &inFile : listOfFiles)
    {
        // all files in list are valid at this point
        fs::path outFile = createOutputFile(inFile, directory, outDirectory);
        fs::path OFCFile = createOFCFile(inFile, directory, OFCDirectory);

        FFExecute::runFFmpeg(stringFromPath(inFile), stringFromPath(outFile), stringFromPath(OFCFile));
        
    }

    deleteDirectoryIfEmpty(outDirectory);
    deleteDirectoryIfEmpty(OFCDirectory);

    printf("Finished all FFmpegs!\n");

    OtherError::printErrors();
}