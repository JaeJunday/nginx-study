#pragma once

namespace server
{
    enum SERVER
    {
        NAME = 0,
        ROOT,
        LISTEN,
        ERROR,
        INDEX,
        MAXBODYSIZE,
        SIZE
    };
}

namespace location
{
    enum LOCATION
    {
        ROOT = 0,
        INDEX,
        AUTOINDEX,
        UPLOAD,
        CGI_PARAM,
        PY,
        BLA,
        PHP,
        CLIENT_MAX_BODY_SIZE,
        LIMIT_EXCEPT,
        TRY_FILES,
        SIZE
    };
}

namespace state
{
	enum AUTOMATA
	{
		SERVER = 1,
        LOCATION = 2,
        PATH = 3,
        KEY = 4,
        VALUE = 5,
        SEMICOLON = 6,
        CLOSE_BRACKET = 7
	};
}

namespace token
{
    enum CKECK_LIST
    {
        SERVER = 1,
        LOCATION = 2,
        PATH = 3,
        OPEN_BRACKET = 4,
        CLOSE_BRACKET = 5,
        SEMICOLON = 6,
        KEY = 7,
        VALUE = 8
    };
}

namespace stack
{
    enum STACK
    {
        SERVER = 1,
        LOCATION = 2,
        OPEN_BRACKET = 4,
        CLOSE_BRACKET = 5
    };
}

namespace request
{
    enum REQUEST
    {
		READY = 0,
		CREATE = 1,
        DONE = 2
    };
}

namespace file
{
    enum STATE
    {
        HASH = 0,
        HEADER = 1,
        CONTENT = 2,
        END = 3,
        START = 4
    };
}

namespace event
{
    enum STATE
    {
        READ = -1,
        WRITE = -2
    };
}

namespace timer
{
    enum TIMER
    {
        TIMEOUT = 30
    };
}

// namespace chunk
// {
//     enum VAR
//     {
//         FIRST = -2,
//     };
//     enum STATE
//     {
//         READY = 0,
//         END = 1,
//         WAIT = 2,
//         INCOMPLETE_DATA = 3,
//         CONTINUE = 4
//     };
// }