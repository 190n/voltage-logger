#!/usr/bin/python3

from sys import argv

if len(argv) < 3:
    print("usage: {} <outputFile> <file1> [<file2>> [...<fileN>]]".format(argv[0]))
    quit()

ofile = argv[1]
files = argv[2:]

csv = "Time (s)," + ",".join(map(lambda s: s + " (mV)", files))

fileContents = {}
for f in files:
    with open(f) as fd:
        fileContents[f] = fd.read()
data = {k: list(map(int, v.split("\n")[:-1])) for k, v in fileContents.items()}

for i in range(max(len(d) for d in data.values())):
    line = "\n" + str(i)
    for idx, f in enumerate(files):
        try:
            line += "," + str(data[f][i])
        except IndexError:
                line += ","
    csv += line

with open(ofile, mode="w") as fd:
    fd.write(csv)
