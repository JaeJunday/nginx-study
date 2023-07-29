#!/bin/bash

# Makefile에서 정의한 프로젝트 이름을 읽어오는 함수
function read_makefile_name() {
    local name_line=$(grep '^NAME\s*=' Makefile)
    if [ -n "$name_line" ]; then
        NAME=$(echo "$name_line" | sed 's/NAME\s*=\s*//')
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
    echo "Compiling..." >&2
    if make all -j; then
        if [ -f "$NAME" ]; then
            echo "Compile succeeded!" >&2
        else
            echo "Compiling..." >&2
        fi
    else
        echo "Compile failed: Make returned non-zero exit code!" >&2
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

# Makefile 감지 실행
watch_makefile
echo "\n"

# 파일 변경 감지 및 컴파일 실행
echo "Compiling is in process." >&2
fswatch -o --exclude 'Makefile' $(find . -name '*.cpp' -o -name '*.hpp') | while read events; do
    compile_file
done &
