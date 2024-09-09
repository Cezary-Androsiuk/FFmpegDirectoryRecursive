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
        printf("No other errors occur.\n");
        return;
    }

    printf("Occur %d other errors:\n", m_errors.size());
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

