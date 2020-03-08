"""
概要；下記形式（タイプ名、領域サイズ、データ内容）のタブ区切りで保存したファイルdata.txtを入力としてバイナリデータdata.binを出力する。
[data.txt]
TYP_STRING	128	12345678901234567890
TYP_UINT8	1	1
TYP_UINT16	2	2
TYP_UINT32	4	3
TYP_INT8	1	4
TYP_INT16	2	5
TYP_INT32	4	6
TYP_BOOL	1	TRUE
"""
import csv
import struct

packIntFunc = lambda size, data, fmt: struct.pack(fmt, int(data))
packStrFunc = lambda size, data, fmt: struct.pack(size + fmt, bytes(data, encoding='utf-8'))
packBoolFunc = lambda size, data, fmt: b'1' if data.lower()=="true" or data == "1" else b'0'

func_tbl = {
    "TYP_STRING"  : [packStrFunc, "s"],
    "TYP_UINT8"   : [packIntFunc, "B"],
    "TYP_UINT16"  : [packIntFunc, "H"],
    "TYP_UINT32"  : [packIntFunc, "L"],
    "TYP_INT8"    : [packIntFunc, "b"],
    "TYP_INT16"   : [packIntFunc, "h"],
    "TYP_INT32"   : [packIntFunc, "l"],
    "TYP_BOOL"    : [packBoolFunc, ""],
}

rf = open("data.txt","r")
wf = open("data.bin","wb")
reader = csv.reader(rf, delimiter='\t')
for row in reader:
    if (not row[0]) or (not row[1]) or (not row[2]):
        continue
    if func_tbl[row[0]]:
        data = func_tbl[row[0]][0](row[1],row[2],func_tbl[row[0]][1])
        print(data)
        wf.write(data)
rf.close()
wf.close()
