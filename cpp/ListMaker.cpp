#include "ListMaker.hpp"

bool ListMaker::vectorContains(cvstr vector, cstr string)
{
    for(const auto &s : vector)
    {
        if(s == string)
        {
            return true;
        }
    }
    return false;
}

vpath ListMaker::listOfFiles(cpath path, cvstr acceptableExtensions, bool printFoundFiles)
{
    if(printFoundFiles)
        printf("Found files:\n");

    vpath list;
    size_t foundFiles = 0;
    size_t filesMatch = 0;
    for(const auto &file : std::filesystem::recursive_directory_iterator(path))
    {
        // printf("\ntesting: %s\n", file.path().string().c_str());
        if(!file.is_regular_file())
            continue;
        ++foundFiles;

        str extension = file.path().extension().string();
        if(!extension.empty())
        {
            if(extension[0] == '.')
                extension.erase(0, 1); // remove dot
        }
        
        if(vectorContains(acceptableExtensions, extension))
        {
            if(printFoundFiles)
                printf("  file nr % 3d: %s\n", foundFiles, file.path().string().c_str());
            list.push_back(file.path());
            ++filesMatch;
        }
    }
    printf("Found %llu / %lld files matching the extensions\n", filesMatch, foundFiles);
    return list;
}