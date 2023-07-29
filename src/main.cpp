#include <iomanip>
#include <iostream>
#include <fstream>
#include <math.h>
#include <sstream>
#include <string>
#include <string.h>
#include <algorithm>
#include <chrono>
#include "MyBloom.h"
#include "MurmurHash3.h"
#include "Rambo_construction.h"
#include "utils.h"
#include "constants.h"
#include "bitArray.h"
#include <ctime>

using namespace std;

int main(int argc, char** argv){

string job(argv[1]);

bool insert  =false;
bool ser =false;
bool test = true;
bool deser = true;

int n_perSet = 1000000000; //cardinality of each set
int R_all = 2;
int B_all = 15;

int K = Ki; // total number of sets

float fp_ops;
float ins_time;
float query_time;

// constructor
RAMBO myRambo(n_perSet, R_all, B_all, K);

//  details of RAMBO set partitioning
myRambo.createMetaRambo (K, false);
cout<<"created meta"<<endl;

//insert itno RAMBO
string SerOpFile ="results/RAMBO_Ser" + to_string(K)+'_'+job +'/';

//string SerOpFile ="results/RAMBO_clueWeb/";

if (insert == true){
  //log files
  std::ofstream failedFiles;
  failedFiles.open("logFileToy_"+ to_string(K)+'_'+job+".txt");
  int stpCnt = 0;
  for (int batch =0; batch<K/100; batch++){
    chrono::time_point<chrono::high_resolution_clock> t3 = chrono::high_resolution_clock::now();

    string dataPath = "data/"+ job +"/inflated/" + to_string(batch) + "_indexed.txt";
    std::vector<string> setIDs = readlines(dataPath, 0);
    int stpt;
	stpt = 5;
    //}
    for (unsigned int ss=0; ss<setIDs.size(); ss++){
      stpCnt++;
      char d = ',';
      vector<std::string> setID = line2array(setIDs[ss], d);
      string mainfile = "data/"+ job +"/inflated/" + setID[1]+ ".out";
      vector<std::string> keys = getctxdata(mainfile);
      failedFiles<<mainfile<<" : "<<keys.size()<<std::endl;

      if (keys.size()==0){
          std::cout<<mainfile<<" does not exists or empty "<<std::endl;
          failedFiles<<mainfile<<" does not exists or empty "<<std::endl;
      }

      myRambo.insertion(setID[0], keys);

    }
    chrono::time_point<chrono::high_resolution_clock> t4 = chrono::high_resolution_clock::now();
    cout << chrono::duration_cast<chrono::nanoseconds>(t4-t3).count()/1000000000.0 << "sec\n";
    ins_time = (t4-t3).count()/1000000000.0;
    failedFiles<<"insertion time (sec) of 100 files: "<<ins_time<<endl;
  }

  //serialize myRambo
  if (ser){
	//string command = "mkdir "+ SerOpFile;
        //system(command.c_str());
	cout<<"Serializing RAMBO at: "<<SerOpFile<<endl;
    myRambo.serializeRAMBO(SerOpFile);
   //gives number of 1s in 30 BFs
   for (int kpp=0;kpp<30;kpp++){
	cout<<"packing: "<<myRambo.Rambo_array[kpp]->m_bits->getcount()<<endl;
	 }
  }
}

if (deser){
  vector<string> SerOpFile2;
  SerOpFile2.push_back(SerOpFile); // mutliple files can be pushed here

  cout<<"deser starting"<<endl;
  myRambo.deserializeRAMBO(SerOpFile2);
  std::cout << "desealized!" << '\n';
}

if(test){
  // test RAMBO
  std::vector<string> alllines = readlines("data/ArtfcKmersToy"+to_string(K)+".txt", 0);
  //std::vector<string> alllines = readlines("data/query.txt", 0);
  std::vector<string> testKeys;
  std::vector<int> gt_size;
  for(unsigned int i=0; i<alllines.size(); i++){
        std::vector<string>KeySets =  line2array(alllines[i], ';');//sets for a key
        testKeys.push_back(KeySets[0]);
        gt_size.push_back( line2array(KeySets[1], ',').size() );
   }
  myRambo.createMetaRambo (K, false);
  // cout<<"load: "<<myRambo.Rambo_array[0]->m_bits->getcount();
  cout<<"total number of queries : "<<testKeys.size()<<endl;
	myRambo.insertion2 (alllines);
	cout<<"loaded test keys"<<endl;

	float fp=0;
	std::ofstream FPtestFile;
	FPtestFile.open("FPtestFileToy.txt");
	std::clock_t t5_cpu = std::clock();
	chrono::time_point<chrono::high_resolution_clock> t5 = chrono::high_resolution_clock::now();

	for (std::size_t i=0; i<testKeys.size(); i++){
			bitArray MemVec = myRambo.query(testKeys[i], testKeys[i].size());
			cout<<MemVec.getcount()<<endl;
			cout<<gt_size[i]<<endl;
			fp = fp + (MemVec.getcount())*0.1/((K - gt_size[i])*0.1);
		}

	cout<<"fp rate is: "<<fp/(testKeys.size()); // false positives/(all negatives)
	FPtestFile<<"fp rate is: "<<fp/(testKeys.size()); // false positives/(all negatives)

	fp_ops = fp/(testKeys.size());

	cout<<endl;
	std::clock_t t6_cpu = std::clock();
	chrono::time_point<chrono::high_resolution_clock> t6 = chrono::high_resolution_clock::now();
	float QTpt_cpu = 1000.0 * (t6_cpu-t5_cpu)/(CLOCKS_PER_SEC*testKeys.size()); //in ms
	float QTpt = chrono::duration_cast<chrono::nanoseconds>(t6-t5).count()/(1000000.0*testKeys.size());
	cout <<"query time wall clock is :" <<QTpt <<", cpu is :" <<QTpt_cpu<< " milisec\n\n";
	FPtestFile<<"query time wall clock is :" <<QTpt <<", cpu is :" <<QTpt_cpu<< " milisec\n\n";
	query_time = QTpt_cpu;
  }

return 0;
}
