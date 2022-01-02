#pragma once

#include <cstdint>

struct LinePos
{
    int32_t m_line = 0;
    int32_t m_col  = 0;

    void nextLine()
    {
        m_line++;
        m_col = 0;
    }

    constexpr bool isValid() const noexcept
    {
        return (m_col != 0) && (m_line != 0);
    }
};

enum class SymbolType
{
    UNKNOWN = 0,
    INTEGER
};
