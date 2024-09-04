#ifndef TEST_PIPE_HPP
#define TEST_PIPE_HPP

#include <cstdio>
#include <string>
#include <filesystem>
#include <iostream>

// ,,Since a Microsoft compiler is used, an underscore is needed at the beginning''
// https://stackoverflow.com/questions/38876218/execute-a-command-and-get-output-popen-and-pclose-undefined
#define wpipeOpen(...) _wpopen(__VA_ARGS__)
#define pipeClose(...) _pclose(__VA_ARGS__)

namespace fs = std::filesystem;

typedef std::string str;
typedef const str &cstr;
typedef const fs::path &cpath;

class TestPipe
{
    static void handleOutput(cstr line);

public:
    static bool testName(cpath path);
    static void setHandleDirOutput(void (*func)(cstr));

private:
    static void (*m_addTextToFFOFile)(cstr);
};



#endif // TEST_PIPE_HPP