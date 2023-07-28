import os

def parse_multipart_data(data, boundary):
    parts = data.split(boundary)
    del parts[0]
    del parts[-1]
    files = []
    for part in parts:
        header, content = part.split("\n\n", 1)
        content = content.replace("\n\n", "")
        headers = header.split('\n')
        del headers[0]
        for line in headers:
            if "filename=" in line:
                start_index = header.find('filename="') + len('filename="')
                end_index = header.find('"', start_index)
                filename = header[start_index:end_index]
            if "Content-Type:" in line:
                content_type = line.split(": ")[1].split(";")[0]
        if header and content:
                files.append((filename, content_type, content))
    return files

def create_files(files, output_dir="."):
    for filename, content_type, content in files:
        file_path = os.path.join(output_dir, filename)
        with open(file_path.decode(), "wb") as file:
            file.write(content)

if __name__ == "__main__":
    data = b"""
------WebKitFormBoundaryI9BGvGlAfS2IafJH
Content-Disposition: form-data; name="name"; filename="test.txt"
Content-Type: text/plain

------------<start>-------------
-------------<end>--------------

------WebKitFormBoundaryI9BGvGlAfS2IafJH
Content-Disposition: form-data; name="name"; filename="test2.txt"
Content-Type: text/plain

------------<start>-------------
-------------<end>--------------

------WebKitFormBoundaryI9BGvGlAfS2IafJH--
"""
    output_directory = "./public"
    boundary = "------WebKitFormBoundaryI9BGvGlAfS2IafJH"
    parsed_files = parse_multipart_data(data, boundary)
    for filename, content_type, content in parsed_files:
        print(filename)
        print(content_type)
        print(content)
    create_files(parsed_files, output_directory)
