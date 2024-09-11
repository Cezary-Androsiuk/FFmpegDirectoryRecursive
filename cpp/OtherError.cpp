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
        printf(COLOR_GREEN "In meantime, no other errors occured" COLOR_RESET ".\n");
        return;
    }

    printf(COLOR_RED "In meantime, " COLOR_WHITE "%d" COLOR_RED 
        " other errors occured" COLOR_RESET ":\n", m_errors.size());
    int index = 0;
    for(const auto &error : m_errors)
    {
        printf("% 5d - %ls\n", ++index, error.c_str());
    }
}

cvwstr OtherError::getErrors()
{
    return m_errors;
}

