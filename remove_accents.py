from unidecode import unidecode
from sys import stdin, argv

n = int(argv[1])
freq = {}
for line in stdin:
    ngram, val = line.split()
    val = int(val)
    ngram = unidecode(ngram)
    if len(ngram) != n:
        continue
    if not ngram in freq:
        freq[ngram] = 0
    freq[ngram] += val
for ngram, val in freq.items():
    print(ngram, val)
