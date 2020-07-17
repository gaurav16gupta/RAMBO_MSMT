import string
import numpy as np
from collections import defaultdict
import csv


parts = ['AA', 'AB','AC']

def getTokens():
    words = set()
    I = open ("wordDict", 'w')
    for part in parts:
        d = open ("doc_words_"+ part, 'r')
        full = d.read()
        docs = full.split('\n')
        for doc in docs:
            text = doc.split(' ')
            if (len(text)>1):
                words = words.union(set(text[1].split(',')))
                # print (text[1].split(','))
        d.close()
    words = '\n'.join(list(words))

    I.write(words)
    I.close()

getTokens()

def TestSet():
    d = open ("efficiency_topics", 'r')
    full = d.read()
    test = full.split('\n')
    # import random
    # test = random.sample(words, 1000)
    GT = defaultdict(set)

    cntr = 0
    for part in parts:
        d = open ("doc_words_"+ part, 'r')
        full = d.read()
        docs = full.split('\n')
        for doc in docs:
            text = doc.split(' ')
            if (len(text)>1):
                for word in test:
                    if word in text[1].split(','):
                        GT[word].add(cntr)
                    else:
                        GT[word] = set()
            cntr+=1
    # print (GT)

    w = csv.writer(open("efficiency_topicsGT.csv", "w"))
    for key, val in GT.items():
        w.writerow([key, val])

    for key in GT:
        print(key, len(GT[key]))

TestSet()
