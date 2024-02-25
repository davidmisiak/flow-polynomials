import sys
import collections

assert len(sys.argv) == 2
with open(sys.argv[1], 'r') as f:
    lines = f.readlines()

agg = collections.defaultdict(int)
for line in lines:
    line = line.strip()
    if line == '': continue
    cols = line.split(' ')
    val, count = ' '.join(cols[:-1]), int(cols[-1])
    agg[val] += count

for k, v in sorted(agg.items()):
    print(k, v)
