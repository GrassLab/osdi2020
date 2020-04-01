#!/usr/bin/python3

import serial
import sys

def openFile( readFile ):
    with open( readFile, "rb" ) as F:
        img = b""
        bb = F.read(1)
        while bb != b"":
            img += bb
            bb = F.read(1)
        return img


s = serial.Serial( sys.argv[2], 115200 )
img_File = openFile( sys.argv[1] )
img_Size = len( img_File )
print( 'Image Size: ' + str(img_Size) )
img_base = int(0x20000).to_bytes( 4, 'little' )
print( 'rebase address : ' + str(img_base) )
print( 'image length : ' + str(img_Size.to_bytes( 4, 'little' )) )