#!/bin/bash

# ANSI escape 코드를 사용하여 터미널 출력 색상 및 스타일 설정
Black='\033[0;30m'
Red='\033[0;31m'
Green='\033[0;32m'
Yellow='\033[0;33m'
Blue='\033[0;34m'
Purple='\033[0;35m'
Cyan='\033[0;36m'
White='\033[0;37m'
B_Black='\033[0;90m'
B_Red='\033[0;91m'
B_Green='\033[0;92m'
B_Yellow='\033[0;93m'
B_Blue='\033[0;94m'
B_Purple='\033[0;95m'
B_Cyan='\033[0;96m'
B_White='\033[0;97m'
Bold='\033[1m'
Italic='\033[3m'
Reset='\033[0m'

# Makefile에서 정의한 프로젝트 이름을 읽어오는 함수
function read_makefile_name() {
    local name_line=$(grep -E '^NAME\s*=' Makefile)
    if [ -n "$name_line" ]; then
        NAME=$(echo "$name_line" | sed -E 's/^NAME\s*=\s*//; s/^[[:space:]]+//')
    else
        echo "Error: NAME not defined in Makefile!" >&2
        exit 1
    fi
}

# Makefile에서 정의한 컴파일러 및 컴파일 플래그를 읽어오는 함수
function read_makefile_flags() {
    local cxx_line=$(grep '^CXX\s*=' Makefile)
    if [ -n "$cxx_line" ]; then
        CXX=$(echo "$cxx_line" | sed 's/CXX\s*=\s*//')
    else
        echo "Error: CXX not defined in Makefile!" >&2
        exit 1
    fi

    local cxxflags_line=$(grep '^CXXFLAGS\s*=' Makefile)
    if [ -n "$cxxflags_line" ]; then
        CXXFLAGS=$(echo "$cxxflags_line" | sed 's/CXXFLAGS\s*=\s*//')
    fi
}

# 컴파일 함수
function compile_file() {
    echo -e "${Blue}Compiling...${Blue}" >&2
    if make all -j; then
        local name_line=$(grep -E '^NAME\s*=' Makefile)
        NAME=$(echo "$name_line" | sed -E 's/^NAME\s*=\s*//; s/^[[:space:]]+//')
        THIRD=$(echo "$NAME" | awk '{print $3}')
        if [ -f "$THIRD" ]; then
            echo -e "${Green}${Italic}Compile Success${Reset}" >&2
        else
            echo -e "${B_Red}Compile failed: Executable '$THIRD' not found!" >&2
        fi
    else
        echo -e "${B_Purple}Compile failed: non-zero exit code!${Reset}" >&2
    fi
}

# Makefile 감지 함수
function watch_makefile() {
    fswatch -o $(find . -name 'Makefile') | while read events; do
        read_makefile_name
        read_makefile_flags
        make fclean
        compile_file
    done
}

# 파일 변경 감지 및 컴파일 실행
echo "Compiling is in process." >&2
fswatch -o --exclude 'Makefile' $(find . -name '*.cpp' -o -name '*.hpp') | while read events; do
    compile_file
done &

# Makefile 감지 실행
watch_makefile
