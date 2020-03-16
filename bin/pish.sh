if [ "$#" -ne 1 ] || [ ! -f "$1" ]; then
    echo "Usage: $0 [SCRIPT FILE]" >&2
    exit 1
fi

if [ ! -e /dev/ttyUSB0 ];then
    echo "ttyUSB0 not found. Did you connect your device?" >&2
    exit 1
fi

for l in `cat test.sh`;do
    sudo sh -c "echo $l | tr -s '\n' '\r' > /dev/ttyUSB0;"
    sleep 0.1;
done
