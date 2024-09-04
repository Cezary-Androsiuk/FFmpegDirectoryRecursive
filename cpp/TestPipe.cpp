#include "TestPipe.hpp"

void (*TestPipe::m_addTextToFFOFile)(cstr);

void TestPipe::handleOutput(cstr line)
{
    if(m_addTextToFFOFile != nullptr)
        m_addTextToFFOFile(line);
}

bool TestPipe::testName(cpath path)
{
    std::wstring command(L"dir \"" + path.wstring() + L"\" 2>&1");
    
    FILE* pipe = wpipeOpen(command.c_str(), L"r");
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