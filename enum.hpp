#pragma once


namespace server
{
    enum SERVER
    {
        NAME = 0,
        LISTEN,
        ERROR,
        INDEX,
        MAXBODYSIZE,
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
        TRY_FILES
    };
}

namespace token
{
    enum TOKEN
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

namespace state
{
	enum STATE
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
