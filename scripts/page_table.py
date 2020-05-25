import sys

if len(sys.argv) < 3:
    print ('py start end')
    sys.exit(0)

start = int(sys.argv[1], 16)
end = int(sys.argv[2], 16)
levels = [39, 30, 21, 12]
result = ''
for addr in [start, end]:
    for l in levels:
        result += hex((addr >> l) & 0x1ff) + '\t'
    result += '\n'
print('PGD\tPUD\tPMD\tPTE')
print('39\t30(1G)\t21(2M)\t12(4K)')
print(result)
