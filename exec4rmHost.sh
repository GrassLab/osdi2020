if [ "$#" -ne 1 ] || ! [ -f "$1" ]; then
    echo "Usage: $0 [SCRIPT FILE]" >&2
    exit 1
fi

if ! [ -f /dev/ttyUSB0 ];then
    echo "ttyUSB0 not found. Did you connect your device?" >&2
    exit 1
fi

cat $1 > /dev/ttyUSB0
