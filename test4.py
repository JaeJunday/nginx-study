import sys
import os

class HTTPFormData:
    def __init__(self, filename, content_type, data):
        self.filename = filename
        self.content_type = content_type
        self.data = data

def create_files(parsed_data, output_directory):
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)

    for data in parsed_data:
        filename = data.filename
        content_type = data.content_type
        file_data = data.data

        output_path = os.path.join(output_directory, filename)
        with open(output_path, 'wb') as file:
            file.write(file_data)

def parse_multipart_formdata(body, boundary):
    boundary = boundary.encode('utf-8')  # 바운더리를 바이트로 변환
    parts = body.split(boundary)
    form_data_list = []

    for part in parts:
        if part.strip() and part != b"--\r\n":
            headers_end = part.find(b'\r\n\r\n')
            if headers_end != -1:
                headers = part[:headers_end].decode('utf-8')
                content = part[headers_end + 4:]
                headers_lines = headers.strip().split('\r\n')
                content_type_line = [line for line in headers_lines if line.startswith('Content-Type')]
                if content_type_line:
                    content_type = content_type_line[0].split(': ')[1].strip()
                else:
                    content_type = None
                name_line = [line for line in headers_lines if line.startswith('Content-Disposition') and 'name=' in line]
                if name_line:
                    name = name_line[0].split('; ')[1].split('=')[1].strip('"')
                else:
                    name = None
                filename_line = [line for line in headers_lines if line.startswith('Content-Disposition') and 'filename=' in line]
                if filename_line:
                    filename = filename_line[0].split('; ')[2].split('=')[1].strip('"')
                else:
                    filename = None
                form_data = HTTPFormData(filename, content_type, content)
                form_data_list.append(form_data)

    return form_data_list

# 바운더리 변수
# boundary = '------WebKitFormBoundary6m96NktxJuonr108'
boundary = os.environ.get("BOUNDARY")
output_directory = "./public"
# 테스트를 위한 HTTP 본문 예제 (바이트로 변환)
# http_body = (f'{boundary}\r\nContent-Disposition: form-data; name="name"; filename="test.txt"\r\nContent-Type: text/plain\r\n\r\n#######################\r\n#######################\r\n#######################\r\n#######################\r\n#######################\r\n#######################\r\n#######################\r\n#######################\r\n{boundary}\r\nContent-Disposition: form-data; name="name"; filename="test2.txt"\r\nContent-Type: text/plain\r\n\r\n#######################\r\n#######################\r\n#######################\r\n#######################\r\n#########0##############\r\n#######################\r\n#######################\r\n#######################\r\n{boundary}--').encode('utf-8')
http_body = sys.stdin.read().encode('utf-8')

# 함수 호출하여 객체로 변환
parsed_data = parse_multipart_formdata(http_body, boundary)

# 결과 출력
for data in parsed_data:
    print(f"Filename: {data.filename}")
    print(f"Content-Type: {data.content_type}")
    data_lines = data.data.decode('utf-8').strip()
    print("Data: ")
    print(data_lines)
    print("\n")
create_files(parsed_data, output_directory)