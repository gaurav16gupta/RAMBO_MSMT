
# RAMBO Fast Processing and Querying of 170TB of Genomics Data via a Repeated And Merged BloOm Filter (RAMBO)
RAMBO is a method to reduce the query cost of sequence search over the archive of dataset files to address the sheer scale and explosive increase of new sequence files.
It solves achives sublinear query time (O(\sqrt{K} log K)) in number of files with memory requirement of slightly more then the information theoretical limit. 

This code is the implementation of: 
[https://dl.acm.org/doi/10.1145/3448016.3457333](https://dl.acm.org/doi/10.1145/3448016.3457333)
for gene sequence search.

If you use RAMBO in an academic context or for any publication, please cite our paper:
```
@inproceedings{10.1145/3448016.3457333,
author = {Gupta, Gaurav and Yan, Minghao and Coleman, Benjamin and Kille, Bryce and Elworth, R. A. Leo and Medini, Tharun and Treangen, Todd and Shrivastava, Anshumali},
title = {Fast Processing and Querying of 170TB of Genomics Data via a Repeated And Merged BloOm Filter (RAMBO)},
year = {2021},
isbn = {9781450383431},
publisher = {Association for Computing Machinery},
address = {New York, NY, USA},
url = {https://doi.org/10.1145/3448016.3457333},
doi = {10.1145/3448016.3457333},
pages = {2226–2234},
numpages = {9},
keywords = {information retrieval, bloom filter, genomic sequence search},
location = {Virtual Event, China},
series = {SIGMOD/PODS '21}
}
```

Step 1: data download
Requirement: 
Install latest GNU parallel OS X:
run:
	brew install parallel

Debian/Ubuntu:
run:
	sudo apt-get install parallel

RedHat/CentOS:
run:
	sudo yum install parallel

Install wget and bzip2

Install cortexpy Refer to this installation [document] (https://cortexpy.readthedocs.io/en/latest/overview.html#installation)

run:
	unzip data/0.zip
	sh data/0/downoad.sh
	mkdir -p results/RAMBOSer_100_0 results/RAMBOSer_200_0 results/RAMBOSer_500_0 results/RAMBOSer_1000_0 results/RAMBOSer_2000_0

In the end we need to execute commands from 0_1.txt > 0_2.txt > 0_3.txt for the 100 files. 

Step 2: ensure all 100 files are present in data/0/inflated/

Step 3: create test set
run:
	python3 artificialKmer.py

Step 4: Set parameters and run code
number of sets in line 7 of include/constants.h
m, B and R in line 29-31 of src/main.cpp
run:
	make
	./build/program 0
