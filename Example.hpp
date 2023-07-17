#pragma once

class Example
{
    private:
    public:
        Example(void);
        Example(const Example& src);
        virtual ~Example(void);
        Example& operator=(Example const& rhs);
};

#include "Example.hpp"

Example::Example(void)
{
    /* Constructor Implementation */
}

Example::Example(const Example& src)
{
    /* Copy Constructor Implementation */
}

Example::~Example(void)
{
    /* Destructor Implementation */
}

Example& Example::operator=(Example const& rhs)
{
    if (this != &rhs)
    {
        /* Assignment Operator Implementation */
    }
    return *this;
}