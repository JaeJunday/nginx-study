#!/usr/local/bin/python3
import sys
import os

class ChunkedData:
    def __init__(self, filename, content_type, data):
        self.filename = filename
        self.content_type = content_type
        self.data = data

def create_files(chunked_data, output_directory):
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)

    # 바이너리 파일 처리
    if chunked_data.content_type and '/' in chunked_data.content_type:
        chunked_data.filename = f"{chunked_data.filename}"
    output_path = os.path.join(output_directory, chunked_data.filename)
    with open(output_path, 'wb') as file:
        file.write(chunked_data.data)

if __name__ == "__main__":

    try:
        # 바운더리 변수
        output_directory = "./public"
        # filename = os.environ.get("FILENAME")
        content_type = os.environ.get("CONTENT_TYPE")
        # 테스트를 위한 HTTP 본문 예제 (바이트로 변환)
        http_body = sys.stdin.buffer.read()
        chunked_data = ChunkedData(filename, content_type, http_body)
        # 파일 생성
        if len(http_body) == 0:
            print("HTTP/1.1 200 OK\r\nDate: Mon, 07 Aug 2023 17:15:02 GMT\r\nServer: SpiderMen/1.5.2\r\nConnection: keep-alive\r\nContent-Length: 19\r\nContent-Type: text/plain\r\n\r\nNo data received.")
        else:
            create_files(chunked_data, output_directory)
            header = "HTTP/1.1 200 OK\r\nDate: Mon, 07 Aug 2023 17:15:02 GMT\r\nServer: SpiderMen/1.5.2\r\nConnection: keep-alive\r\n"
            body = "<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<title>Success Page</title>\r\n</head>\r\n<body>\r\n<h1>Success!</h1>\r\n<p>Your Post request was successful.</p>\r\n</body>\r\n</html>"
            content_length = len(body)
            response = header + "Content-Length: " + str(content_length) + "\r\n" + "Content-Type: text/html\r\n\r\n" + body
            print(response, end='')
        sys.exit(0)

    except Exception as e:
        print(f"ERROR IN CGI.PY: {str(e)}", file=sys.stderr)
        sys.exit(1)
