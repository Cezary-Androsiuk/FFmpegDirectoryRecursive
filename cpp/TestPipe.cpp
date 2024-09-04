#include "TestPipe.hpp"

void (*TestPipe::m_addTextToFFOFile)(cstr);

void TestPipe::handleOutput(cstr line)
{
    if(m_addTextToFFOFile != nullptr)
        m_addTextToFFOFile(line);
}

str TestPipe::stringFromPath(cpath path)
{
    std::wstring ws = path.wstring();
    return std::string(ws.begin(), ws.end());
}

bool TestPipe::testName(cpath path)
{
    str command("dir \"" + TestPipe::stringFromPath(path) + "\" 2>&1");
    
    FILE* pipe = pipeOpen(command.c_str(), "r");
    if (!pipe) {
        fprintf(stderr, "failed to open pipe\n");
        return false;
    }

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
        try
        {
            TestPipe::handleOutput(str(buffer));
        }
        catch(const std::exception& e)
        {
            printf("error while handling output in TestPipe\n");
        }
    }

    int exitCode = pipeClose(pipe);
    return (exitCode ? false : true);
}

void TestPipe::setHandleDirOutput(void (*func)(cstr))
{
    m_addTextToFFOFile = func;
}