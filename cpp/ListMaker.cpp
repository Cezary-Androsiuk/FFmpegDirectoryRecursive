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

vpath ListMaker::listOfFiles(cpath path, cvstr acceptableExtensions)
{
    vpath list;
    int index = 0;
    for(const auto &file : std::filesystem::directory_iterator(path))
    {
        ++index;
        if(!file.is_regular_file())
            continue;

        str extension = file.path().extension().string();
        extension.erase(0, 1); // remove dot
        
        if(vectorContains(acceptableExtensions, extension))
        {
            printf("  file nr % 3d: %s\n", index, file.path().string().c_str());
            list.push_back(file.path());
        }
    }
    return list;
}