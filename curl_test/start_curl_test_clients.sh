#!/usr/bin/bash

# gzipコマンド、curlコマンドによるPOST[圧縮]通信
echo "start POST by 'gzip' command and 'curl' command"
echo '{"name":"Test"}' | gzip -c | curl --compressed --data-binary @- -H "Content-Encoding: gzip" -H "Content-Type: application/json" --trace http_debug_by_cmd.txt http://127.0.0.1:5000/post

# zlib, libcurl をソースコード上で使用してPOST通信
echo "start POST by source code with zlib and libcurl"
./curl_test_client 2>http_debug_by_src.txt
