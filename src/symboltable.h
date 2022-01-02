#pragma once

#include <unordered_map>
#include <optional>

#include "mytypes.h"

struct Symbol
{
    std::string m_name;

    SymbolType  m_type;     ///< symbol type
    size_t      m_offset;   ///< offset within the owning stack frame
};

class SymbolTable
{
public:

    bool addSymbol(const std::string &name, SymbolType _type);
    bool addSymbol(const Symbol &symbol);

    Symbol* getSymbol(const std::string &name);
    const Symbol* getSymbol(const std::string &name) const;

protected:
    size_t currentAllocOffset = 0;
    bool hasSymbol(const std::string name) const;
    std::unordered_map<std::string, Symbol> m_symbols;
};
