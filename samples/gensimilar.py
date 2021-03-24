
import sys

if len(sys.argv) < 2:
    print('usage: gensimilar.py num', file=sys.stderr)
    sys.exit(1)

for i in range(int(sys.argv[1])):
    print('word{}'.format(i+1))
