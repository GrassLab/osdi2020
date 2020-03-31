import sys
import numpy as np
import cv2

header_only = True

def usage():
    print("usage: {} [image] > img.c".format(sys.argv[0]))
    exit(0)

if len(sys.argv) <= 1 or ('-h' in sys.argv) or ('--help' in sys.argv):
    usage()

filename = sys.argv[1]
img = cv2.imread(filename)

if type(img) == type(None):
    print('cannot read "{}" as image'.format(filename))
    exit(1)

header = open("raspi.h", "w")
header.write('/* convert from {} */\n'.format(filename))
header.write("""
#define raspi_height {}
#define raspi_width {}
\n""".format(img.shape[0], img.shape[1]))

if not header_only:
    header.write('extern char *raspi_data[];'.format(filename))

if header_only:
    source = header
else:
    source = open("raspi.c", "w")

source.write('/* convert from {} */\n'.format(filename))
source.write("char *raspi_data[] = {\n")
for c in [2, 1, 0]:
    source.write('"')
    for y in range(img.shape[0]):
        for x in range(img.shape[1]):
            source.write("\\{}".format(hex(img[y][x][c])[1:]))
    source.write('"')
    if c: source.write(",\n")
    else: source.write("\n")
source.write("};")

header.close()
if not header_only: source.close()
