import numpy as np
# import matplotlib.pyplot as plt

gene = 'ATGC'
kmers = []

maxV = 10
intervel = 100 #number of files inserted in rambo

f = open("ArtfcKmersToy"+str(intervel)+".txt", "w")

for j in range(0,1000): #these many queries
    a = ''
    for i in range(0,10):
        a = a + gene[np.random.randint(4, size=1)[0]]
    for k in range(0,1): #for 1 merges
        maxV = int((intervel-1)*np.random.exponential(0.01, 1)) +1
        #maxV =1
        print (maxV)
        VI = np.random.randint(intervel*k, intervel*(k+1), maxV) # maxV is max V
        a = a + ';'+ ','.join(['%d' % num for num in VI])
    f.write(a + '\n')
f.close()
