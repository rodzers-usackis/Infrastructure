#!/usr/bin/env python3
# Take care of the indentation! 
from http.server import BaseHTTPRequestHandler, HTTPServer
import time
hostName = "127.0.0.1"
hostPort = 8765
class MyServer(BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200)
        self.send_header("Content-type","text/html") 
        self.end_headers()
        self.wfile.write(bytes("This is a python web server","utf-8"))
myServer = HTTPServer((hostName, hostPort), MyServer)
print(time.asctime(), "Server Starts - %s:%s" % (hostName, hostPort))
try:
    myServer.serve_forever()
except KeyboardInterrupt:
    pass
myServer.server_close()
print(time.asctime(), "Server Stops - %s:%s" % (hostName, hostPort))