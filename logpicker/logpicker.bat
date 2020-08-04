@cd %~dp0 & python -d -x "%~f0" %* & pause & exit /b %errorlevel%

## Pythonスクリプトはここから ##
import re
import csv

print("テキストファイルのパスを入力してください")
target_txt = input()
target_file = open(target_txt, "r", encoding='utf-8')

print("検索キーワードのタブ区切りファイル(.tsv)を入力してください")
key_txt = input()
key_file = open(key_txt, "r", encoding='utf-8')
tsv_rdr = csv.reader(key_file, delimiter='\t')

out_file = "result_log.txt"
print("解析結果をファイル名[", out_file, "]で出力します")

for i, line_str in enumerate(target_file):
    line_str = line_str.strip()
    for row in tsv_rdr:
        m = re.search(row[0],line_str)
        if m:
            print("{:d}行目>".format(i+1), line_str)
            print("    ⇒", row[1].format(*m.groups()))
