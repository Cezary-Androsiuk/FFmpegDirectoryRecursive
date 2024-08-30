#include "TestPipe.hpp"

str TestPipe::pathToString(cpath path)
{
    std::wstring w(path.wstring());
    return str(w.begin(), w.end());
}

bool TestPipe::testName(cpath path)
{
    str command("dir \"" + TestPipe::pathToString(path) + "\" 2>&1");

    FILE* pipe = pipeOpen(command.c_str(), "r");
    if (!pipe) {
        fprintf(stderr, "failed to open pipe\n");
        return false;
    }

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
    { }

    int exitCode = pipeClose(pipe);
    return (exitCode ? false : true);

}