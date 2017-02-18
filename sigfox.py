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

PORT = 8000

class myHandler(hs.SimpleHTTPRequestHandler):
    data = []
    action = ''
    def do_GET(self):
        self.send_response(200, 'ok')
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        get = parse_qs(self.path[2:])
        #myHandler.data.extend([hex_to_int(get['data'][0][i:i+2]) for i in range(0, 12, 2)])
        #plt.plot([int(get['time'][0])+0.2*i for i in range(12)], [hex_to_int(get['data'][0][i:i+2]) for i in range(0, 24, 2)])
        #plt.plot(myHandler.data, [x - time[0] for x in time])
        if 'acc_x' in get:
            myHandler.action = get['action'][0]
            myHandler.data.extend(get['acc_x'])
            plt.clf()
            tmp = myHandler.data[-2000:]
            plt.plot(range(len(tmp)), myHandler.data[-2000:])
            plt.ylabel('blue')
            plt.gca().set_ylim([-2000, 2000])
            plt.savefig('test.png', figsize=(4, 100))
        elif 'png' in self.path or 'jpg' in self.path:
            with open(self.path[1:], 'rb') as fp:
                self.wfile.write(fp.read())
        else:
            with open('index.html', 'rb') as fp:
                tmp = fp.read().replace(b'ACTION', myHandler.action.encode('ascii')).replace(b'IMG', myHandler.action.encode('ascii'))
                self.wfile.write(tmp)
        return

httpd = socketserver.TCPServer(("", PORT), myHandler)
httpd.serve_forever()
