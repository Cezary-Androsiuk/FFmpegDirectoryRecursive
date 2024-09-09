#include "TemporaryRename.hpp"

bool TemporaryRename::m_temporaryRenamed;
fs::path TemporaryRename::m_inFileOld;
fs::path TemporaryRename::m_inFileTmp;
fs::path TemporaryRename::m_outFileOld;
fs::path TemporaryRename::m_outFileTmp;
fs::path TemporaryRename::m_moveFileOld;
fs::path TemporaryRename::m_moveFileTmp;

str TemporaryRename::getCurrentTime()
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

str TemporaryRename::makeRandomFilename(str extension)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 10000); 
    long long random = std::chrono::milliseconds(dis(gen)).count();
    str randomString = std::to_string(random);
    str currentTime = TemporaryRename::getCurrentTime();
    str randomCurrentTime;
    int minSize = randomString.size() > currentTime.size() ? currentTime.size() : randomString.size();
    for(int i=0; i<minSize; i++)
        randomCurrentTime += std::to_string(randomString[i] ^ currentTime[i]);

    return currentTime + "_" + randomString + "_" + randomCurrentTime + extension;
}

fs::path TemporaryRename::createValidAlternativeName(cpath path)
{
    fs::path parenPath = path.parent_path();
    str extension = path.extension().string();
    fs::path newPath;
    do{
        if(!newPath.empty())
            printf("file '%s' already exist, looking for the next one...\n", newPath.string().c_str());
        str newFilename = TemporaryRename::makeRandomFilename(extension);
        newPath = parenPath / newFilename;
    }
    while(fs::exists(newPath));

    // printf("found valid filename '%s'\n", newPath.string().c_str());
    return newPath;
}

bool TemporaryRename::tryRename(cpath from, cstr fromName, cpath to, cstr toName, bool otherError)
{
    try
    {
        if(!fs::exists(from))
        {
            fprintf(stderr, "%s " COLOR_RED "not exit" COLOR_RESET " '%ls', can't rename to %s '%ls'\n", 
                fromName.c_str(), from.wstring().c_str(), toName.c_str(), to.wstring().c_str());
            if(otherError)
                ADD_OTHER_ERROR(toWideString(fromName) +L" not exit '" + from.wstring() + 
                    L"', can't rename to "+ toWideString(toName) +L" '" + to.wstring() + L"'");
            return false;
        }
        if(fs::exists(to))
        {
            fprintf(stderr, "%s " COLOR_RED "already exit" COLOR_RESET " '%ls', can't rename %s to it '%ls'\n", 
                toName.c_str(), to.wstring().c_str(), fromName.c_str(), from.wstring().c_str());
            if(otherError)
                ADD_OTHER_ERROR(toWideString(toName) +L" already exit '" + to.wstring() + 
                    L"', can't rename "+ toWideString(fromName) +L" to it '" + from.wstring() + L"'");
            return false;
        }
        fs::rename(from, to);
    }
    catch(const std::exception& e)
    {
        fprintf(stderr, COLOR_RED "error while renaming" COLOR_RESET " %s '%ls' to %s '%ls' : %s\n", 
            fromName.c_str(), from.wstring().c_str(), toName.c_str(), to.wstring().c_str(), e.what());
        if(otherError)
            ADD_OTHER_ERROR(L"error while renaming "+ toWideString(fromName) +L"'" + from.wstring() + 
                L"' to "+ toWideString(toName) +L" '" + to.wstring() + L"' : " + toWideString(e.what()));
        return false;
    }

    // that was a nightmare
    return true;
}

bool TemporaryRename::makeNameSimple(fs::path &inFile, fs::path &outFile, fs::path &moveFile)
{
    m_inFileOld = inFile;
    m_outFileOld = outFile;
    m_moveFileOld = moveFile;

    // printf("m_inFileOld: %s\n", m_inFileOld.string().c_str());
    m_inFileTmp = TemporaryRename::createValidAlternativeName(inFile);
    if(!TemporaryRename::tryRename(m_inFileOld, "inFileOld", m_inFileTmp, "inFileTmp"))
        return false;

    inFile = m_inFileTmp;
    // printf("m_inFileTmp: %s\n", m_inFileTmp.string().c_str());
    
    // printf("m_outFileOld: %s\n", m_outFileOld.string().c_str());
    m_outFileTmp = TemporaryRename::createValidAlternativeName(outFile);
    outFile = m_outFileTmp;
    // printf("m_outFileTmp: %s\n", m_outFileTmp.string().c_str());
    
    // printf("m_moveFileOld: %s\n", m_moveFileOld.string().c_str());
    m_moveFileTmp = TemporaryRename::createValidAlternativeName(moveFile);
    moveFile = m_moveFileTmp;
    // printf("m_moveFileTmp: %s\n", m_moveFileTmp.string().c_str());

    return true;
}

bool TemporaryRename::restoreName(fs::path &inFile, fs::path &outFile, fs::path &moveFile)
{

    // in file propably not exist, because it should be moved to moveFile
    if(fs::exists(m_inFileTmp))
    {
        if(!TemporaryRename::tryRename(m_inFileTmp, "inFileTmp", m_inFileOld, "inFileOld", true))
            return false;
    }
    inFile = m_inFileOld;
    
    
    if(!TemporaryRename::tryRename(m_outFileTmp, "outFileTmp", m_outFileOld, "outFileOld", true))
        return false;
    outFile = m_outFileOld;
    
    if(!TemporaryRename::tryRename(m_moveFileTmp, "moveFileTmp", m_moveFileOld, "moveFileOld", true))
        return false;
    moveFile = m_moveFileOld;
    
    return true;
}

wstr TemporaryRename::getRenameInfo()
{
    wstr info;
    info = L"{\n";
    info += L"  '" + m_inFileOld.wstring() + L"' -> '" + m_inFileTmp.wstring() + L"',\n";
    info += L"  '" + m_outFileOld.wstring() + L"' -> '" + m_outFileTmp.wstring() + L"',\n";
    info += L"  '" + m_moveFileOld.wstring() + L"' -> '" + m_moveFileTmp.wstring() + L"'\n}\n";
    return info;
}