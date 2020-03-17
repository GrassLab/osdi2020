import sys
import time

if __name__ == '__main__':

    if len(sys.argv) != 2:
        print('usage: script.py <command file>')
        sys.exit(1)

    try:
        with open('/dev/ttyUSB0', 'w') as port, open(sys.argv[1]) as input:
            for line in input:
                port.write(line)
                time.sleep(0.1)
                if 'reboot' in line:
                    time.sleep(3)
    except PermissionError as e:
        print('Please check you have the permission to ttyUSB0 and script file')
