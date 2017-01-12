#!/usr/bin/python3

import collections
import datetime
import os
import re

class Row(collections.namedtuple('Row',
    ['world', 'level', 'internal', 'score', 'whom'])):

    def id(self):
        return '{:02}-{:02}'.format(self.world, self.level)

    def percentile(self, bins):
        for idx, bin in enumerate(bins):
            if self.score > bin.bin_start:
                return idx/len(bins)

    def level_name(self):
        return self.internal[:-4]

class Bin(collections.namedtuple('Bin',
    ['bin_start', 'bin_end', 'amount'])):
    pass

def parse(input_file):
    pat = re.compile('^(\d+)-(\d+) (\w+) (\d+) (.*)')
    with open(input_file, 'r') as f:
        for line in f:
            ma = pat.search(line)
            if not ma:
                continue
            yield Row(int(ma.group(1)),
                    int(ma.group(2)),
                    ma.group(3),
                    int(ma.group(4)),
                    ma.group(5))

def by_level(input_file):
    ret = collections.defaultdict(list)
    for row in parse(input_file):
        ret[row.id()].append(row)

    for scores in ret.values():
        scores.sort(key=lambda x: x.score)

    return ret


def cached_stats():
    import json
    import os
    with open(os.path.expanduser('~/.config/unity3d/Dry Cactus/Poly Bridge/cached_stats.json')) as f:
        for level in json.load(f):
            yield (level['level'], [Bin(
                float(x['bin_start']),
                float(x['bin_end']),
                int(x['amount'])
                ) for x in level['budget']])

def main(input_file):
    read = by_level(input_file)
#    import json
#    import sys
#    json.dump(by_level(), sys.stdout)
    print('Leaders:')
    winners = sorted(collections.Counter(x[0].whom for x in read.values()).items(), key=lambda x: -x[1])
    complete = collections.Counter(y.whom for x in read.values() for y in x)
    for winner in winners:
        print('{:5}/{:2} {}'.format(winner[1], complete[winner[0]], winner[0]))

    stats = dict(cached_stats())
#    people = sorted({y.whom for x in read.values() for y in x})
#    print(people)
#    print(stats['Easy_001'])
#    print(read['01-01'])

    print()

    for level in sorted(read.keys()):
        det = read[level]
        first = det[0]
        print('{}-{} ({}):'.format(first.world, first.level, first.internal))
        for row in det:
            #print(row.percentile(stats[row.level_name()]), row.whom)
            print('{:5.1f}% {}'.format(100*(row.score-first.score)/first.score, row.whom))

        print()

    print('generated: {}'.format(datetime.datetime.fromtimestamp(os.path.getmtime(input_file)).isoformat()))



if "__main__" == __name__:
    import sys
    main(sys.argv[1])
