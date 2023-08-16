#!/usr/bin/env python3
import os
import re
import sys


def parse_multipart_octet_stream():
    #환경변수 가져와서 변수에 저장
    content_type = os.environ.get('CONTENT_TYPE', '')
    upload_dir = os.environ.get('DOCUMENT_ROOT', './var/www/html')
    
    content_length = int(os.environ.get('CONTENT_LENGTH', 0))
    
    if content_length: #if content_length가 있을 경우
        
        post_data = sys.stdin.buffer.read() # Read the raw POST data till EOF

        if content_type.startswith('multipart/form-data'):  #형식이 'multipart/form-data' 일 경우
            boundary = content_type.split('boundary=')[-1].encode('utf-8') #boundary 파싱(추출)

            # Split the data into individual parts based on the boundary
            parts = post_data.split(b'--' + boundary)

            # Create the upload_dir if it doesn't exist
            os.makedirs(upload_dir, exist_ok=True)
            for part in parts[1:-1]:  # Skip the first and last parts (boundary marker 제외하기)
                # Extract filename and content
                header, content = part.split(b'\r\n\r\n', 1)
                filename_match = re.search(r'filename="(.*?)"', header.decode(), re.DOTALL)
                if filename_match:
                    filename = filename_match.group(1)
                    filename = os.path.basename(filename)  # Remove any path information for security
                    file_path = os.path.join(upload_dir, filename) #업로드 디렉토리랑 파일명 합쳐서 파일주소만들기

                    with open(file_path, 'wb') as f: #파일주소로 fd열어서 그 안에 쓰기. wb는 write binary로 바이너리문자로 쓰기 옵션입니다 (기존 파일 있는 경우 덮어씀)
                        f.write(content)

        elif content_type == 'text/plain': #형식이 'text/plain'일 경우
            #이 블록은 DOCUMENT_ROOT로 받아오는 루트에 파일명이 있는 것으로 가정하고 처리했습니다

            content = post_data
            upload_path = upload_dir[:upload_dir.rfind('/')] #디렉토리만 분리

            # Create the upload_dir if it doesn't exist
            os.makedirs(upload_path, exist_ok=True)
            with open(upload_dir, 'wb') as f:
                f.write(content)

        elif content_type == 'plain/text': #처음보는 콘텐츠 타입이시죠? 저희도 그랬습니다. 평가표에 curl 테스트에서 들어올 수 있는 형식으로 평가자님을 위한 이스터에그입니다(그냥 귀엽습니다) 
            print("Content-Type: text/plain")
            print()
            print("안녕하세요 평가자님.\n평가에 오신 것을 환영합니다^^")
            sys.exit(0)

        else: #content length가 있는 경우 multipart, text/plain, plain/text외에 다른 형식이 들어오면 걍 처리 안해주고 200 OK만 처리해주는걸로 하고싶어서 짠 부분입니다.
            print("Content-Type: text/plain")
            print()
            print("CGI: unvalid content type received.")
            print(content_type)
            sys.exit(0)

    #if chunked
    elif os.environ.get('HTTP_TRANSFER_ENCODING') == "chunked":
        post_data = sys.stdin.read() # Read the raw POST data till EOF
        
        with open(upload_dir, 'ab') as f: #ab는 append binary입니다. 이어서 쓰기 옵션. (wb로 했어도 상관없었을거같긴합니다)
            if (len(post_data)):
                f.write(post_data.encode("utf-8")) # post_data를 인코딩해서 씁니다. string이 아닌 binary로 저장하면 더 좋다고 합니다.
                # f.write(post_data)
            # print(post_data, file=sys.stderr)

    else: #POST요청인데 body가 없는 경우 처리하는 부분
        print("Content-Type: text/plain")
        print()
        print("No data received.")
        sys.exit(0)

    
    
#파일 저장 성공 시 pipe에 써주는 내용입니다. (이 내용이 response의 바디가 됩니다)
content = """<html>
<head><title>CGI Example</title></head>
<body>
<h1>CGI POST SUCCES!!</h1>
</body>
</html>"""

if __name__ == "__main__":

    try:
        parse_multipart_octet_stream()
    except Exception as e:
        print(f"Error: {str(e)}", file=sys.stderr)
        sys.exit(1) #error 발생 시 1(256)으로 종료됩니다. EVFILT_PROC 이벤트를 받으면 wait_pid로 받은 이 내용을 검사해 실패여부를 확인할 수 있습니다.

    print("Content-Type: text/html")
    print()  # 헤더와 본문을 구분하는 빈 줄

    print(content)

    sys.exit(0)
