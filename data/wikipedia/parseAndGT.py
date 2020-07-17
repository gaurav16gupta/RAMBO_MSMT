from nltk.corpus import stopwords
from nltk.tokenize import word_tokenize
from collections import defaultdict
import csv
import string
import numpy as np
import glob, os

cachedStopWords = stopwords.words("english")
table = str.maketrans('', '', string.punctuation)

dir = 'text/'
parts = ['AA', 'AB','AC']

def cleanFull(text):
    # split into words
    tokens = word_tokenize(text)
    # convert to lower case
    tokens = [w.lower() for w in tokens]
    # remove punctuation from each word
    stripped = [w.translate(table) for w in tokens]
    # remove remaining tokens that are not alphabetic
    words = [word for word in tokens if word.isalpha()]
    # filter out stop words
    words = list(set([w for w in words if w not in cachedStopWords]))
    # print (len(words))
    # words = ','.join(words)
    return words
    # text = ' '.join([word for word in text.split() if word not in cachedStopWords])

def PageName(id):
    return id.split('>')[1][1:]



for part in parts:

    testTokenpath = open ("efficiency_topics", 'r')
    testTokens = testTokenpath.read()
    testTokens = testTokens.split('\n')[:-1]
    GT = defaultdict(set)
    for word in testTokens:
        GT[word] = set()

    docID =0

    for filename in glob.glob(dir + part+ "/*"):
        with open(filename) as f:
            full = f.read()
        docs = full.split(r'</doc>')[:-1]
        for doc in docs:
            [id,text] = doc.split('\n\n')[0:2]
            # print (text)
            # print (cleanFull(text))
            text = cleanFull(text)
            for word in testTokens:
                if word in text:
                    GT[word].add(docID)


            docID+=1
        for key in GT:
            print(key, len(GT[key]))

    w = csv.writer(open("efficiency_topicsGT.csv", "w"))
    for key, val in GT.items():
        w.writerow([key, val])

    gttxt = open ("efficiency_topicsGT.txt", 'w')
    # gttxt = gttxtpath.read()
    for key in GT:
        gttxt.write(key+";"+ str(len(GT[key]))+"\n")
        print(key, len(GT[key]))
        # d.close()
        # I.close()
