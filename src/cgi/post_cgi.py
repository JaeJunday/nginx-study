#!/usr/local/bin/python3
import sys
import os
import re

def get_filename():
    filepath = os.environ.get("DOCUMENT_ROOT")
    lastpart = filepath.split("/")[-1]
    return lastpart

def handle_chunked(output_directory):
    post_data = sys.stdin.buffer.read() # Read the raw POST data till EOF
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)
    filename = get_filename()
    output_path = os.path.join(output_directory, filename)
    with open(output_path, 'wb') as file:
        file.write(post_data)

def parse_data(output_directory):
    boundary = os.environ.get("BOUNDARY")
    content_type = os.environ.get("CONTENT_TYPE")
    content_length = int(os.environ.get("CONTENT_LENGTH", 0))
    post_data = sys.stdin.buffer.read()

    if not os.path.exists(output_directory):
        os.makedirs(output_directory)
    if content_length:
        if content_type == 'text/plain' or content_type == 'plain/text':
            filename = get_filename()
            output_path = os.path.join(output_directory, filename)
            with open(output_path, 'wb') as file:
                file.write(post_data)
        elif content_type.startswith('multipart/form-data'):
            parts = post_data.split(boundary.encode('utf-8'))
            for part in parts[1:-1]:
                header, content = part.split(b'\r\n\r\n', 1)
                filename_match = re.search(r'filename="(.*?)"', header.decode(), re.DOTALL)
                if filename_match:
                    filename = filename_match.group(1)
                    filename = os.path.basename(filename)
                    output_path = os.path.join(output_directory, filename) 
                with open(output_path, 'wb') as file:
                    file.write(content)
        else:
            print("State: 200 OK\r\n", end='')
            print("Content-Type: text/plain\r\n\r\n", end='')
            print("CGI: unvalid content type received.\r\n", end='')
            print(content_type, end='')
            sys.exit(0)
    else:
        print("State: 200 OK\r\n", end='')
        print("Content-Type: text/plain\r\n\r\n", end='')
        print("No data received.")
        sys.exit(0)

output_directory = "./public"
cgi_body = "<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<title>Success Page</title>\r\n</head>\r\n<body>\r\n<h1>Success!</h1>\r\n<p>Your Post request was successful.</p>\r\n</body>\r\n</html>"

if __name__ == "__main__":

    try:
        if os.environ.get('HTTP_TRANSFER_ENCODING') == "chunked":
            handle_chunked(output_directory)
        else:
            parse_data(output_directory)
    except Exception as e:
        print(f"ERROR IN CGI.PY: {str(e)}", file=sys.stderr)
        sys.exit(1)
    try:
        print("State: 200 OK\r\n", end='')
        print("Content-Type: text/html\r\n\r\n", end='')
        print(cgi_body, end='')
    except Exception as e:
        print(f"PRINT ERROR IN CGI.PY: {str(e)}", file=sys.stderr)
        sys.exit(1)
    sys.exit(0)