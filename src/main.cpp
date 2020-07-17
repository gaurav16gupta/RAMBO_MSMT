#include <iomanip>
#include <iostream>
#include <fstream>
#include <math.h>
#include <sstream>
#include <string>
#include <string.h>
#include <algorithm>
#include <chrono>
// #include "MyBloom.h"
#include "MurmurHash3.h"
#include "Rambo_construction.h"
// #include "ArrayOfBF.h"
#include "utils.h"
#include "constants.h"
#include "bitArray.h"
#include <ctime>

using namespace std;

int main(int argc, char** argv){

string job(argv[1]);

bool insert  = false;
bool insert_toy = false;

bool doInsert = true;
bool doTest =  true;

bool insert_Doc= false;
bool test= false;
bool deser= false;
bool ser= false;
if (doInsert){ insert_Doc = true;  ser = true;}
if (doTest){ test = true; deser = true;}

//debug
int n_perSet = 200000; //cardinality of each set
int R_all = 2;
int B_all = 1000;

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
string SerOpFile ="results/RAMBO_wiki/";
string dataPathsFile = "data/wikipedia/dataPathsFile.txt";

if (insert){
  //log files
  std::ofstream failedFiles;
  failedFiles.open("logFileB500R5_"+job+".txt");

  for (int batch =0; batch<47; batch++){
    chrono::time_point<chrono::high_resolution_clock> t3 = chrono::high_resolution_clock::now();

    string dataPath = "data/"+ job +"/inflated/" + to_string(batch) + "_indexed.txt";
    std::vector<string> setIDs = readlines(dataPath, 0);

    for (uint ss=0; ss<setIDs.size(); ss++){
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
}


if (insert_Doc){
  //log files
  int batchSize = 0;
  int cntr = 0;

  std::ofstream logFile;
  logFile.open("logFile_"+to_string(batchSize)+".txt");
  std::vector<string> dataPaths = readlines(dataPathsFile, 0);

  chrono::time_point<chrono::high_resolution_clock> t3 = chrono::high_resolution_clock::now();
  std::clock_t t5_cpu = std::clock();
  for (uint dp=0; dp<dataPaths.size(); dp++){

    std::vector<string> docData = readlines(dataPaths[dp], 0); //0 means reall all lines
    for (uint ss=0; ss<docData.size(); ss++){
      // char d = ',';
      if (line2array(docData[ss], ' ').size()<2){logFile<<"exception"<<endl; continue;}
      vector<std::string> keys = line2array(line2array(docData[ss], ' ')[1], ',');

      int InsSize = keys.size();
      logFile<<"number of ins: "<<InsSize<<endl;
      if ((cntr>batchSize) & batchSize){break;}
      if (InsSize==0){
          std::cout<<to_string(ss)<<" does not exists or empty "<<std::endl;
          logFile<<to_string(ss)<<" does not exists or empty "<<std::endl;
      }

      myRambo.insertion(to_string(cntr), keys);
      cntr+=1;
    }
  }
  std::clock_t t6_cpu = std::clock();
  chrono::time_point<chrono::high_resolution_clock> t4 = chrono::high_resolution_clock::now();
  cout << chrono::duration_cast<chrono::nanoseconds>(t4-t3).count()/1000000000.0 << "sec\n";
  float ins_time_cpu = (t6_cpu-t5_cpu)/CLOCKS_PER_SEC; //in sec
  ins_time = (t4-t3).count()/1000000000.0;
  logFile<<"insertion time (sec): "<<ins_time<<"insertion time CPU (sec): "<<ins_time_cpu<<endl;
}


  //serialize myRambo
if (ser){
	cout<<"Serializing RAMBO at: "<<SerOpFile<<endl;
    myRambo.serializeRAMBO(SerOpFile);

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
  std::vector<string> alllines = readlines("data/wikipedia/efficiency_topicsGT.txt", 0); //0 means all

  std::vector<string> testKeys;
  std::vector<int> gt_size;

  for(uint i=0; i<alllines.size(); i++){
        std::vector<string>KeySets =  line2array(alllines[i], ';');//sets for a key
        testKeys.push_back(KeySets[0]);
        gt_size.push_back( stoi(KeySets[1]));
   }

  myRambo.createMetaRambo (K, false);
  cout<<"created meta"<<endl;

  cout<<"total : "<<testKeys.size()<<endl;

  // myRambo.insertion2 (alllines);
  // cout<<"loaded keys"<<endl;

  float fp=0;
  std::ofstream FPtestFile;
  FPtestFile.open("FPtestFileToy.txt");
  chrono::time_point<chrono::high_resolution_clock> t5 = chrono::high_resolution_clock::now();
  std::clock_t t5_cpu = std::clock();
  for (std::size_t i=0; i<testKeys.size(); i++){
          bitArray MemVec = myRambo.query(testKeys[i], testKeys[i].size());
          // int gt_size[i] = V;
          fp = fp + (MemVec.getcount())*0.1/((K - gt_size[i])*0.1);
  }
  chrono::time_point<chrono::high_resolution_clock> t6 = chrono::high_resolution_clock::now();
  std::clock_t t6_cpu = std::clock();
  cout<<"fp rate is: "<<fp/testKeys.size(); // false positives/(all negatives)
  FPtestFile<<"fp rate is: "<<fp/testKeys.size(); // false positives/(all negatives)

  fp_ops = fp/testKeys.size();;
  cout<<"total # of queries: "<<testKeys.size()<<endl;
  cout<<endl;
  cout <<"query time  is :" <<chrono::duration_cast<chrono::nanoseconds>(t6-t5).count()/(1000000.0*testKeys.size()) << "msec\n";
  cout <<"query cpu time  is :" <<(1000.0*(t6_cpu-t5_cpu))/(CLOCKS_PER_SEC*testKeys.size()) << "msec\n";
  FPtestFile<<"query time is :" <<chrono::duration_cast<chrono::nanoseconds>(t6-t5).count()/1000000.0 << "msec\n";
  query_time = (t6-t5).count()/1000000000.0;
  cout<<"CLOCKS_PER_SEC: "<<CLOCKS_PER_SEC<<endl;

  }
return 0;
}
