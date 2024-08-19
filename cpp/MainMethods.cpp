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

bool argsValid(int argc, const char **argv, fs::path *const directory, vstr *const extensions, SkipAction *const skipAction)
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

    // very fancy way to check it xd (by Gemini)
    return fs::directory_iterator(directory) == fs::directory_iterator();
}

bool createOutputDirectory(fs::path outDirectory)
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
    }
    else
    {
        // "ffmpeg-h.265" not exist, now try to create it
        if(!fs::create_directory( outDirectory ))
        {
            lastError = "Failed while creating output directory: " + outDirectory.string();
            return false;
        }
    }

    return true;
}

fs::path createOutputFilename(cpath inFile, cpath outDirectory)
{
    str filename = inFile.filename().string();

    size_t dotPos = filename.find_last_of('.');
    if(dotPos == str::npos)
        return outDirectory / (filename + ".mp4");
    return outDirectory / (filename.substr(0, dotPos+1) + "mp4");
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
            fs::remove( outDirectory );
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