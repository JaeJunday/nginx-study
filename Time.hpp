#pragma once

#include <iostream>
#include <ostream>
#include <string>
#include <ctime>

#define TIME_SIZE 40

class Time
{
    private:
        Time(const Time& src);
        Time& operator=(Time const& rhs);
        Time();
        virtual ~Time(void);
    public:
        static void stamp();
};
