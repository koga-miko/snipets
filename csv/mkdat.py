"""
概要；バイナリデータdata.binをフォーマットテキストformat.txtに基づいて解析してデータテキストdata2.txtへ出力する
[format.txt]
TYP_STRING	128
TYP_UINT8	1
TYP_UINT16	2
TYP_UINT32	4
TYP_INT8	1
TYP_INT16	2
TYP_INT32	4
TYP_BOOL	1
[data2.txt]
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
from os import path
import re

unpackIntFmtFunc = lambda size, fmt: fmt
unpackStrFmtFunc = lambda size, fmt: size + fmt
convertIntFunc = lambda intdat: str(intdat)
convertBytesFunc = lambda bytesdat: re.sub(r'\x00.*', '', str(bytesdat, encoding='utf-8'))

func_tbl = {
    "TYP_STRING"  : [unpackStrFmtFunc, "s", convertBytesFunc],
    "TYP_UINT8"   : [unpackIntFmtFunc, "B", convertIntFunc],
    "TYP_UINT16"  : [unpackIntFmtFunc, "H", convertIntFunc],
    "TYP_UINT32"  : [unpackIntFmtFunc, "L", convertIntFunc],
    "TYP_INT8"    : [unpackIntFmtFunc, "b", convertIntFunc],
    "TYP_INT16"   : [unpackIntFmtFunc, "h", convertIntFunc],
    "TYP_INT32"   : [unpackIntFmtFunc, "l", convertIntFunc],
    "TYP_BOOL"    : [unpackIntFmtFunc, "?", convertIntFunc],
}

rff = open("format.txt","r")
rbf = open("data.bin","rb")
wtf = open("data2.txt","w")
fmt_reader = csv.reader(rff, delimiter='\t')
txt_writer = csv.writer(wtf, delimiter='\t')
cnt = 1
binpos = 0
for row in fmt_reader:
    if (not row[0]) or (not row[1]):
        print("err:L.{}, invalid line contents.".format(cnt))
        break
    if not func_tbl[row[0]]:
        print("err:L.{}, invalid type:{}".format(cnt, row[0]))
        break
    fmt = func_tbl[row[0]][0](row[1],func_tbl[row[0]][1])
    size = int(row[1])
    if not (size == struct.calcsize(fmt)):
        print("err:L.{}, size missmatch. size:{}, fmt:{}".format(cnt, row[1], fmt))
        break
    bindat = rbf.read(size)
    if not size == len(bindat):
        print("err:L.{}, rbf.read size error. :size:{}, binpos:{}, len(bindat):{}".format(cnt, row[1], binpos, len(bindat)))
        break
    dat = struct.unpack(fmt, bindat)
    writedat = [row[0],row[1],func_tbl[row[0]][2](dat[0])]
    print(writedat)
    txt_writer.writerow(writedat)
    cnt = cnt + 1
    binpos = binpos + size
binfilesize = path.getsize("data.bin")
if binfilesize != binpos:
    print("err:, total size missmatch. binfilesize:{}, fmt total size:{}".format(binfilesize, binpos))
rff.close()
rbf.close()
wtf.close()
