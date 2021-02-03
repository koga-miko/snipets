# coding utf-8
import csv
import sys

def parse_dat(datfile, outcsv):
    with open(datfile,"rb") as f:
        wf = open(outcsv,"w",encoding='cp932')
        csv_writer = csv.writer(wf, dialect='excel', lineterminator="\n")
        csv_writer.writerow(['Sys time', 'Recog text', 'Cmd ID'])
        while True:
            data = f.read(16)
            if len(data) < 16:
                break
            if data[3] == 0x33:
                data2 = f.read(32)
                if len(data2) < 32:
                    break
                data3 = data[14:16] + data2[8:16] + data2[20:32]
                time = (data[0]*65536+data[1]*256+data[2]) * 0.05
                str = data3.decode('utf-8')
                id = data[12]*256 + data[13]
                csv_writer.writerow([time, str, id])
        wf.close()

if __name__ == '__main__':
    #if len(sys.argv) < 2:
    #    exit(1)
    # parse_dat(sys.argv[1], sys.argv[2])
    parse_dat("in.dat", "out.csv")