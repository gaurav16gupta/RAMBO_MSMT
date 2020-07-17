from nltk.corpus import stopwords
from nltk.tokenize import word_tokenize
import string
import numpy as np
import glob, os

cachedStopWords = stopwords.words("english")
table = str.maketrans('', '', string.punctuation)

dir = '/home/gaurav/Documents/RAMBO_latest/RAMBO/data/wikipedia/text/'
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
    words = ','.join(words)
    return words
    # text = ' '.join([word for word in text.split() if word not in cachedStopWords])

def PageName(id):
    return id.split('>')[1][1:]

for part in parts:
    d = open ("doc_words_"+ part, 'w')
    I = open ("doc_ID_"+ part, 'w')

    docID =0
    for filename in glob.glob(dir + part+ "/*"):
        with open(filename) as f:
            full = f.read()
        docs = full.split(r'</doc>')[:-1]
        for doc in docs:
            [id,text] = doc.split('\n\n')[0:2]
            # print (text)
            # print (cleanFull(text))
            d.write(str(docID)+" "+ cleanFull(text)+"\n")
            I.write(str(docID)+":"+ PageName(id)+"\n")
            docID+=1

    d.close()
    I.close()
