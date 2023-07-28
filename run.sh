#!/bin/bash

# 이전 파일의 수정 시간 기록
prev_mtime=""

while true; do
    # 파일 경로
    file_path="$(pwd)/test.txt"

    # 파일이 존재하는지 확인
    if [ -e "$file_path" ]; then
        # 현재 파일의 수정 시간 가져오기
        current_mtime=$(date -r "$file_path" +%s)

        # 이전 파일의 수정 시간과 현재 파일의 수정 시간 비교
        if [ "$prev_mtime" != "$current_mtime" ]; then
            # 파일 변경이 감지되었을 때 스크립트 실행
            python3 ./test3.py

            # 이전 파일의 수정 시간 업데이트
            prev_mtime="$current_mtime"
        fi
    fi

    # 1초 대기
    sleep 1
done
