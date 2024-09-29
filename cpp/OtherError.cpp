#include "OtherError.hpp"

vwstr OtherError::m_errors;

void OtherError::addError(cwstr errorDesc, cstr from)
{
    wstr error = errorDesc + L", from: " + toWideString(from);
    m_errors.push_back(error);
}

void OtherError::printErrors()
{
    if(m_errors.empty())
    {
        printf("In meantime," COLOR_GREEN " no other errors occured" COLOR_RESET ".\n");
        return;
    }

    printf("In meantime, " COLOR_RED "%d other errors occured" COLOR_RESET 
        ":\n", m_errors.size());
    int index = 0;
    for(const auto &error : m_errors)
    {
        printf("  " COLOR_RED "% 5d" COLOR_RESET " - %ls\n", ++index, error.c_str());
    }
}

cvwstr OtherError::getErrors()
{
    return m_errors;
}

