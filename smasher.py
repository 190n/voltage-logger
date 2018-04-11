#!/usr/bin/python3

import csv, sys
from functools import partial
from collections import OrderedDict
from math import ceil

# how many readings to combine into one
chunkSize = 60

def mapper(vals, enum):
    (idx, val) = enum
    if val == None:
        return None
    if idx < smoothDist - 1:
        toAvg = vals[:idx+1]
    else:
        toAvg = vals[(idx - smoothDist + 1):idx+1]
    return sum(toAvg) / len(toAvg)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("usage: {} <infile> [outfile]".format(sys.argv[1]))

    data = OrderedDict()

    with open(sys.argv[1]) as csvfile:
        dr = csv.DictReader(csvfile)
        types = dr.fieldnames[1:]
        for t in types:
            data[t] = []
        for r in dr:
            for t in types:
                data[t].append(int(r[t]) if r[t] != "" else None)

    newData = OrderedDict()
    targetLen = ceil(max(len(l) for l in data.values()) / chunkSize)
    for t in data.keys():
        newData[t] = []
        idx = 0
        while idx < len(data[t]) and data[t][idx] != None:
            toAvg = data[t][idx:idx+chunkSize]
            toAvg = list(filter(lambda n: n != None, toAvg))
            avg = sum(toAvg) / len(toAvg)
            newData[t].append(avg)
            idx += chunkSize
        while len(newData[t]) < targetLen:
            newData[t].append(None)


    if len(sys.argv) == 2:
        ofile = sys.stdout
    else:
        ofile = open(sys.argv[2], "w")
    writer = csv.DictWriter(ofile, ["Time (s)"] + list(newData.keys()))
    writer.writeheader()
    for i in range(len(newData[list(newData.keys())[0]])):
        d = {"Time (s)": i * chunkSize}
        d.update({t: newData[t][i] for t in newData})
        writer.writerow(d)
    if ofile != sys.stdout:
        ofile.close()
