from urllib.parse import parse_qs
import http.server as hs
import socketserver
import sys
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

def hex_to_int(a):
    a = int(a, 16)
    return a if a < 0x80 else a-0x100

PORT = 8080

class myHandler(hs.SimpleHTTPRequestHandler):
    data = []
    action = ''
    def do_GET(self):
        self.send_response(200, 'ok')
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        get = parse_qs(self.path[2:])
        if 'data' in get and get['data'][0] == '01':
            print('EMERGENCY')
        return

httpd = socketserver.TCPServer(("", PORT), myHandler)
httpd.serve_forever()
