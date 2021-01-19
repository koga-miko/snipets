#!/usr/bin/python3
# coding:utf-8
import json
import gzip
import io
from flask import Flask, jsonify, make_response, request, Response

app = Flask(__name__)

@app.route('/post', methods=['POST'])
def post_json():
  resp = make_responsemake_response(request.data)
  resp.headers['Content-Encoding'] = request.headers['Content-Encoding']
  resp.headers['Content-Type'] = request.headers['Content-Type']
  return resp

if __name__ == "__main__":
  app.debug = True
  app.run()

