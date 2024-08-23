#include "OtherError.hpp"

vstr OtherError::m_errors;

void OtherError::addError(cstr errorDesc, cstr from)
{
    str error = errorDesc + ", from: " + from;
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
        printf("% 5d - %s\n", ++index, error.c_str());
    }
}

cvstr OtherError::getErrors()
{
    return m_errors;
}

