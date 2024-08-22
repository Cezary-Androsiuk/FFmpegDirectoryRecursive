#include "MainMethods.hpp"

str lastError;
const char possibleSeparators[] = {',', '/', '\\', /*'|', */';', '+', '?'};

vstr splitStringByChar(cstr str, char separator) 
{
    // by Gemini
    vstr splited;
    size_t start = 0;
    size_t end = str.find(separator);

    while (end != str::npos) {
        splited.push_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(separator, start);
    }

    splited.push_back(str.substr(start, str::npos));

    return splited;
}

vstr splitExtensionsInput(str input)
{
    char usedSeparator;
    for(int i=0; i<sizeof(possibleSeparators); i++)
    {
        char testedSeparator = possibleSeparators[i];
        if(input.find_first_of(testedSeparator) != str::npos)
        {
            usedSeparator = testedSeparator;
            break;
        }
    }
    return splitStringByChar(input, usedSeparator);
}

void stringTolower(str &string)
{
    std::transform(string.begin(), string.end(), string.begin(), 
        [](unsigned char c){ return std::tolower(c); });
}

SkipAction handleInputSkipAction(str input)
{
    stringTolower(input);
    
    if(input == "skip") return SkipAction::Skip;
    else if(input == "move") return SkipAction::Move;
    else if(input == "copy") return SkipAction::Copy;

    // fprintf(stderr, "Unrecognized '%s', available options are skip/move/copy\n");
    return SkipAction::None;
}

bool handleArgs(int argc, const char **argv, void *arguments[])
{
    // very dangerous function... but, i kinda like this risk ;)
    
    fs::path *directory = static_cast<fs::path *>(arguments[0]);
    vstr *extensions = static_cast<vstr *>(arguments[1]);
    SkipAction *skipAction = static_cast<SkipAction *>(arguments[2]);

    bool exitValue;

#if FLEXIBLE_ARGUMENTS
    exitValue = argsValidFlexible(argc, argv, directory, extensions, skipAction);
    if(!exitValue)
    {
        fprintf(stderr, COLOR_RESET "Arguments are not valid:" COLOR_RED " %s\n" COLOR_RESET, lastError.c_str());
        fprintf(stderr, "Expected none, one, two or three arguments!\n");
        printf("ffmpegRec :1 :2 :3\n");
        printf("  :1 - path to execute ffmpeg in it\n");
        printf("  :2 - extensions to look for, can be separated by ,/\\?;+\n");
        printf("  :3 - action when file is already H265 [skip/copy/move]\n");
    }
#else
    exitValue = argsValidConst(argc, argv, directory, extensions, skipAction);
    if(!exitValue)
    {
        fprintf(stderr, COLOR_RESET "Arguments are not valid:" COLOR_RED " %s\n" COLOR_RESET, lastError.c_str());
        fprintf(stderr, "Expected three arguments!\n");
        printf("ffmpegRec :1 :2 :3\n");
        printf("  :1 - path to execute ffmpeg in it\n");
        printf("  :2 - extensions to look for, can be separated by ,/\\?;+\n");
        printf("  :3 - action when file is already H265 [skip/copy/move]\n");
    }
#endif
    return exitValue;
}

bool argsValidConst(int argc, const char **argv, fs::path *const directory, vstr *const extensions, SkipAction *const skipAction)
{
    FUNC_START
    if(argc < 4)
    {
        lastError = "To few arguments!";
        return false;
    }

    fs::path givenDirectory = argv[1];
    vstr givenExtensions = splitExtensionsInput( str(argv[2]) );
    SkipAction givenSkipAction = handleInputSkipAction( str(argv[3]) );
    
    if(!fs::exists( givenDirectory ))
    {
        lastError = "File " + givenDirectory.string() + " not exist!";
        return false;
    }

    if(!fs::is_directory( givenDirectory ))
    {
        lastError = "File " + givenDirectory.string() + " is not a directory!";
        return false;
    }

    if(givenSkipAction == SkipAction::None)
    {
        // don't use default to not force user to stop algorithm (for example if he spell type wrong)
        lastError = "Given argument '" + str( argv[3] ) + "' not match possible options!";
        return false;
    }

    
    if(directory != nullptr)
        *directory = fs::absolute( givenDirectory );

    if(extensions != nullptr)
        *extensions = givenExtensions;

    if(skipAction != nullptr)
        *skipAction = givenSkipAction;

    return true;
}

bool argsValidFlexible(int argc, const char **argv, fs::path *const directory, vstr *const extensions, SkipAction *const skipAction)
{
    FUNC_START

    fs::path givenDirectory;
    vstr givenExtensions;
    SkipAction givenSkipAction;

    switch (argc)
    {
    case 1: // none args
        givenDirectory = DEFAULT_PATH;
        givenExtensions = DEFAULT_EXTENSIONS;
        givenSkipAction = DEFAULT_SKIP_ACTION;
        break;

    case 2: // one argument
        givenDirectory = argv[1];
        givenExtensions = DEFAULT_EXTENSIONS;
        givenSkipAction = DEFAULT_SKIP_ACTION;
        break;

    case 3: // two arguments
        givenDirectory = argv[1];
        givenExtensions = splitExtensionsInput( str(argv[2]) );
        givenSkipAction = DEFAULT_SKIP_ACTION;
        break;

    case 4: // three arguments
    default: // more argumens
        givenDirectory = argv[1];
        givenExtensions = splitExtensionsInput( str(argv[2]) );
        givenSkipAction = handleInputSkipAction( str(argv[3]) );
        break;
    }
    
    if(!fs::exists( givenDirectory ))
    {
        lastError = "File " + givenDirectory.string() + " not exist!";
        return false;
    }

    if(!fs::is_directory( givenDirectory ))
    {
        lastError = "File " + givenDirectory.string() + " is not a directory!";
        return false;
    }

    if(givenSkipAction == SkipAction::None)
    {
        // don't use default to not force user to stop algorithm (for example if he spell type wrong)
        lastError = "Given argument '" + str( argv[3] ) + "' not match possible options!";
        return false;
    }

    if(directory != nullptr)
        *directory = fs::absolute( givenDirectory );

    if(extensions != nullptr)
        *extensions = givenExtensions;

    if(skipAction != nullptr)
        *skipAction = givenSkipAction;

    return true;
}

bool isDirectoryEmpty(fs::path directory)
{
    // should be an directory at this point, then no checking

    for(const auto &file : fs::recursive_directory_iterator(directory))
    {
        if(file.is_directory())
            continue;

        // if not directory, then must be a file => is not empty
        return false;
    }
    return true;
}

bool createDirectoryIfValid(fs::path outDirectory)
{
    FUNC_START

    if(fs::exists( outDirectory ))
    {
        // "ffmpeg-h.265" exist, now check if is directory
        if(!fs::is_directory( outDirectory ))
        {
            lastError = "Output directory already exist, but is not a directory: " + outDirectory.string();
            return false;
        }

        if(!isDirectoryEmpty( outDirectory ))
        {
            // this will help to avoid multiple program execution in the same directory
            lastError = "Output directory already exist, and is not empty: " + outDirectory.string();
            return false;
        }

        // if is empty (not contains files) delete it anyway, it might contains empty folders, that are not exist in inputDirectory
        // directory contains only empty folders at this point
        fs::remove_all( outDirectory );

    }

    // "ffmpeg-h.265" not exist, now try to create it
    if(!fs::create_directory( outDirectory ))
    {
        lastError = "Failed while creating output directory: " + outDirectory.string();
        return false;
    }

    return true;
}

bool copyStructureOfFolders(fs::path sourceDir, fs::path targetDir)
{
    FUNC_START

    str sourceStr = sourceDir.string();
    str targetStr = targetDir.string();

    for(const auto &file : fs::recursive_directory_iterator(sourceDir))
    {
        if(!file.is_directory())
            continue;

        str directoryStr = fs::absolute(file.path()).string();
        if(directoryStr == sourceStr)
            continue;
        
        size_t startPos = directoryStr.find(sourceStr);
        if(startPos == str::npos)
        {
            lastError = "Error while looking for string '" + sourceStr + "' in '" + directoryStr + "'\n";
            return false;
        }
        directoryStr.erase(startPos, sourceStr.size()+1);
        
        fs::path directoryToCreate = targetDir / directoryStr;
        if(fs::exists(directoryToCreate))
            continue;
        
        try{
            fs::create_directories(directoryToCreate);
        }
        catch(std::filesystem::filesystem_error &e)
        {
            lastError = "Error while creating directory: " + directoryToCreate.string() + "! Error: " + e.what();
            return false;
        }
    }

    printf("Structure of folders created\n");

    return true;
}

fs::path createOutputDirectory(cpath inputDirectory, bool removeDirIfExist)
{
    fs::path outDirectory( inputDirectory.string() + "-ffmpeg-h.265" );

    if(removeDirIfExist)
    {
        printf(COLOR_RED "DEBUG ONLY!" COLOR_RESET "\n");
        rm_all(outDirectory);
    }
    
    if(!createDirectoryIfValid( outDirectory ))
    {
        fprintf(stderr, 
            COLOR_RESET "Failed while creating output directory:" 
            COLOR_RED " %s" COLOR_RESET, lastError.c_str());
        return fs::path();
    }

    if(!copyStructureOfFolders(inputDirectory, outDirectory))
    {
        fprintf(stderr, 
            COLOR_RESET "Failed while creating structure of folders in output directory:" 
            COLOR_RED " %s" COLOR_RESET, lastError.c_str());
        return fs::path();
    }
    return outDirectory;
}

fs::path createOCFDirectory(cpath inputDirectory, bool removeDirIfExist) // OCFDirectory is OutputCompletedFilesDirectory
{
    fs::path OFCDirectory( inputDirectory.string() + "-compressed_source_files");

    if(removeDirIfExist)
    {
        printf(COLOR_RED "DEBUG ONLY!" COLOR_RESET "\n");
        rm_all(OFCDirectory);
    }

    if(!createDirectoryIfValid( OFCDirectory ))
    {
        fprintf(stderr, 
            COLOR_RESET "Failed while creating output completed files directory:" 
            COLOR_RED " %s" COLOR_RESET, lastError.c_str());
        return fs::path();
    }

    if(!copyStructureOfFolders(inputDirectory, OFCDirectory))
    {
        fprintf(stderr, 
            COLOR_RESET "Failed while creating structure of folders in output completed files directory:" 
            COLOR_RED " %s" COLOR_RESET, lastError.c_str());
        return fs::path();
    }
    return OFCDirectory;
}

fs::path createOutputFilename(cpath inFile, cpath directory, cpath outDirectory)
{
    // replace extension to mp4 (works then i won't change it to filesystem way)
    str newFilename = inFile.filename().string();
    size_t dotPos = newFilename.find_last_of('.');
    // if file name not contains any dot => no extension also
    newFilename = dotPos == str::npos ? newFilename : newFilename.substr(0, dotPos);
    str changedInFile = (inFile.parent_path() / (newFilename + ".mp4")).string();

    // change to output directory
    size_t startPos = changedInFile.find(directory.string());
    if(startPos == str::npos)
    {
        printf(COLOR_RED "WTF!\n'%s' SHOULD EXIST IN '%s'\n" COLOR_RESET, directory.string().c_str(), changedInFile.c_str());
        printf("EXIT FROM %s\n", __PRETTY_FUNCTION__);
        exit(1);
    }
    changedInFile.erase(startPos, directory.string().size());
    if(!changedInFile.empty())
    {
        if(changedInFile[0] == '\\' || changedInFile[0] == '/')
            changedInFile.erase(0,1);
    }
    return outDirectory / changedInFile;
}

void deleteDirectoryIfEmpty(fs::path outDirectory)
{
    if(fs::exists( outDirectory ))
    {
        // "ffmpeg-h.265" exist, now check if is directory
        if(!fs::is_directory( outDirectory ))
            return;

        if(isDirectoryEmpty( outDirectory ))
        {
            // directory contains only empty folders at this point
            fs::remove_all( outDirectory );
        }
    }
}

bool rm_all(const fs::path& path) {
    try {
        fs::remove_all(path);
        return true;
    } catch (const fs::filesystem_error& e) {
        fprintf(stderr, "Error while deleting structure: %s\n", e.what());
        return false;
    }
}