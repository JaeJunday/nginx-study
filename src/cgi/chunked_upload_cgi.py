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

# 바운더리 변수
output_directory = "./public"
filename = os.environ.get("FILENAME")
content_type = os.environ.get("CONTENT_TYPE")
# 테스트를 위한 HTTP 본문 예제 (바이트로 변환)
http_body = sys.stdin.buffer.read()
chunked_data = ChunkedData(filename, content_type, http_body)
# 파일 생성
create_files(chunked_data, output_directory)
print("CHUNKED DONE!!!!!!")
