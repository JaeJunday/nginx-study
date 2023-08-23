#!/usr/local/bin/python3
import sys

cgi_body = "<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<title>GET CGI Page</title>\r\n</head>\r\n<body>\r\n<h1>GET CGI Success!</h1>\r\n<p>Your GET request was successful.</p>\r\n</body>\r\n</html>"

if __name__ == "__main__":
    print("State: 200 OK\r\n", end='')
    print("Content-Type: text/html\r\n\r\n", end='')
    print(cgi_body)
    sys.exit(0)