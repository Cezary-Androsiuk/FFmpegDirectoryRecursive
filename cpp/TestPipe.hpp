#ifndef TEST_PIPE_HPP
#define TEST_PIPE_HPP

#include <cstdio>
#include <string>
#include <filesystem>

// ,,Since a Microsoft compiler is used, an underscore is needed at the beginning''
// https://stackoverflow.com/questions/38876218/execute-a-command-and-get-output-popen-and-pclose-undefined
#define pipeOpen(...) _popen(__VA_ARGS__)
#define pipeClose(...) _pclose(__VA_ARGS__)

namespace fs = std::filesystem;

typedef std::string str;
typedef const fs::path &cpath;

class TestPipe
{
    static str pathToString(cpath path);

public:
    static bool testName(cpath path);
};



#endif // TEST_PIPE_HPP