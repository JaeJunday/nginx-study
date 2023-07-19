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
        PY,
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
        CLOSE_BRACKET = 5,
    };
}
