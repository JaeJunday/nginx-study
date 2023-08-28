#pragma once

// main.cpp
#define EXIT_DONE 0
#define EXIT_ERROR 1
// operation.cpp
#define SOCKET_TYPE 0
#define MAX_LISTEN 1024
#define MAX_EVENT 1024
// client.cpp
#define TIME_SIZE 40
#define PIPESIZE 42000
#define INIT_PIPE -2
#define INIT_PID -2
// request.cpp
#define HEX 16

namespace server
{
    enum SERVER
    {
        NAME,
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
        ROOT,
        INDEX,
        AUTOINDEX,
        UPLOAD,
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
		READY,
		CREATE,
        DONE
    };
}

namespace file
{
    enum STATE
    {
        HASH,
        HEADER,
        CONTENT,
        END,
        START
    };
}

namespace event
{
    enum STATE
    {
        READ = -1,
        WRITE = -2
    };

    enum KEVENT
    {
    };
}

namespace timer
{
    enum TIMER
    {
        TIMEOUT = 60
    };
}
