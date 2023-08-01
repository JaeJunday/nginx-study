#!/bin/bash

# 파이프로부터 데이터를 읽어옵니다.
data=$(cat)

# 읽어온 데이터를 출력합니다.
echo "Content-Type: text/plain"
echo
echo "$data"
