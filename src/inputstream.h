#pragma once
#include <fstream>

class ISrcStream
{
public:
    /** look at the current character */
    virtual char peek() const = 0;

    /** advance to the next character */
    virtual void next() = 0;

    /** returns true if there are no more chars to read */
    virtual bool atEnd() const = 0;
};

class FileSrcStream : public ISrcStream
{
public:
    FileSrcStream(std::ifstream &is) : m_is(is)
    {      
        next();  
    }

    char peek() const override
    {
        return m_curChar;
    }

    void next() override
    {
        if (atEnd())
        {
            m_curChar = 0;
        }
        else
        {
            m_is.get(m_curChar);
        }
    }

    bool atEnd() const override
    {
        if (m_is.eof())
        {
            return true;
        }

        return false;
    }

protected:
    std::ifstream &m_is;
    char m_curChar;
};
