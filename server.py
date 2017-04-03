#!/usr/bin/env python

from http.server import BaseHTTPRequestHandler, HTTPServer
import ast
import os

class TestHTTPRequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        return self.do_GEToPOST()

    def do_POST(self):
        return self.do_GEToPOST()

    def do_GEToPOST(self):
        rootdir = './'
        self.urls = {
                "sum": self.srv_sum,
                "avg": self.srv_avg
        }
        uri = self.path.split('/')[1].split('?')[0]
        if(self.urls[uri]):
            self.urls[uri]()
            return
        else:
            self.resp404()
        

    def params(self):
        if(self.command == 'GET'):
            args = self.path.split('?', 1)
            if(len(args) > 1):
                params = args[1].split('&')
                pdict = dict(item.split('=') for item in params)
                peval = dict((k,ast.literal_eval(v)) for k,v in pdict.items())
                return peval
            else:
                return "" 
        else:
            content_len = int(self.headers.get('content-length', 0))
            post_body = self.rfile.read(content_len).decode()
            return ast.literal_eval(post_body)

    def srv_sum(self):
        p0 = self.params()
        if not 'input' in p0:
            self.resp404('usage: HOST/srv?input=a1[,a2,a3...]')
            return
        p = p0['input']
        s = sum(p)
        self.resp200(str(s))
    def srv_avg(self):
        p0 = self.params()
        if not 'input' in p0:
            self.resp404('usage: HOST/srv?input=a1[,a2,a3...]')
            return
        p = p0['input']
        a = sum(p)/len(p)
        self.resp200(str(a))

    def resp200(self, message):
        self.send_response(200)
        self.send_header('Content-type','text-html')
        self.end_headers()
        self.wfile.write(str.encode(message or 'ok'))
    def resp404(self, message='file not found'):
        self.send_error(404, message)


def run():
    print('http server starting...')

    server_address = ('127.0.0.1', 8080)
    httpd = HTTPServer(server_address, TestHTTPRequestHandler)
    print('http server running...')
    httpd.serve_forever()

if __name__ == '__main__':
    run()
