#include "symboltable.h"

bool SymbolTable::addSymbol(const std::string &name, SymbolType _type)
{
    return addSymbol({name, _type});
}

bool SymbolTable::addSymbol(const Symbol &symbol)
{
    if (hasSymbol(symbol.m_name))
    {
        return false; // already exists!    
    }

    m_symbols[symbol.m_name] = symbol;

    //FIXME: when we have other types besides integers, 
    //       the offset increment will change!
    m_symbols[symbol.m_name].m_offset = currentAllocOffset;
    currentAllocOffset += 2;
    return true;
}

bool SymbolTable::hasSymbol(const std::string name) const
{
    auto iter = m_symbols.find(name);
    if (iter == m_symbols.end())
    {
        return false;
    }

    return true;
}

Symbol* SymbolTable::getSymbol(const std::string &name)
{
    auto iter = m_symbols.find(name);
    if (iter == m_symbols.end())
    {
        return nullptr;        
    }

    return &(iter->second);
}

const Symbol* SymbolTable::getSymbol(const std::string &name) const
{
    auto iter = m_symbols.find(name);
    if (iter == m_symbols.end())
    {
        return nullptr;        
    }

    return &(iter->second);    
}
