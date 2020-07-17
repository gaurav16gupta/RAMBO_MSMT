#include <iomanip>
#include <fstream>
#include <iostream>
#include <chrono>
#include <vector>
#include <math.h>
#include <sstream>
#include <string>
#include <string.h>
#include <algorithm>
// #include "MyBloom.h"
#include "MurmurHash3.h"
#include "Rambo_construction.h"
#include "utils.h"
#include "constants.h"
#include "bitArray.h"
#include <set>
#include <iterator>
// #include <bitset>

using namespace std;

vector<uint> RAMBO:: hashfunc( std::string key, int len){
  // int hashvals[k];
  vector <uint> hashvals;
  uint op; // takes 4 byte
  if (R==2){
    MurmurHash3_x86_32(key.c_str(), len, 1, &op); //seed i
    hashvals.push_back((op )%B);
    hashvals.push_back((op >>8 )%B);
  }
  else{
    for (int i=0; i<R; i++){
      MurmurHash3_x86_32(key.c_str(), len, i, &op); //seed i
      hashvals.push_back(op%B);
    }}
  return hashvals;
}

vector<uint> myhash( std::string key, int len, int k, int r, int range){
  // int hashvals[k];
  vector <uint> hashvals;
  uint op; // takes 4 byte
  MurmurHash3_x86_32(key.c_str(), len, r, &op);
  for (int i=0+r*k; i<k +r*k; i++){
    hashvals.push_back((op>>(2*i))%range);
  }
  return hashvals;
}

std::vector <std::string>  RAMBO:: getdata(string filenameSet){

  //get the size of Bloom filter by count
  ifstream cntfile (filenameSet);
  std::vector <std::string> allKeys;
  int totKmerscnt = 0;
  while ( cntfile.good() )
      {
          string line1, vals;
          while( getline ( cntfile, line1 ) ){
              stringstream is;
              is<<line1;
              if (line1[0]!= '>' && line1.size()>30 ){
                for (uint idx =0; idx<line1.size()-31 +1; idx++){
                  // std::cout<<line1.substr(idx, 31)<<std::endl;
                  allKeys.push_back(line1.substr(idx, 31));
                  totKmerscnt++;
                }
              }
          }
      }
      std::cout<<"total inserted from one file: "<<totKmerscnt<<std::endl;
      return allKeys;
  }


RAMBO::RAMBO(int n, int r1, int b1, int K){
  // p = fpr1;
  // FPR = fpr1;
  R = r1;
  B = b1;
  K = K;
  k = 3;
  //range = ceil(-(n*log(p))/(log(2)*log(2))); //range
  range = n;
  std::cout << "range" <<range<< '\n';

  int sz = range*B*R/8 + 1;
  Rambo_array  = new char[sz];
  for (int i=0; i<sz; i++ ){
    Rambo_array[i] = '\0'; // Clear the bit array
  }

  // Rambo_array = new BloomFiler*[B*R]; //array of pointers .....dynamically allocated
  // // std::vector<BloomFiler>  Rambo_array(B*R, BloomFiler(n, p, range, k));
  metaRambo = new vector<int>[B*R]; //constains set info in it.
}
// one time process- a preprocess step
void RAMBO::createMetaRambo(int K, bool verbose){
  for(int i=0;i<K;i++){
    vector<uint> hashvals = RAMBO::hashfunc(std::to_string(i), std::to_string(i).size()); // R hashvals, each with max value B
    for(int r=0; r<R; r++){
      metaRambo[hashvals[r] + B*r].push_back(i);
    }
  }
  //print RAMBO meta deta
  if (verbose){
    for(int b=0; b<B; b++){
      for(int r=0; r<R; r++){
        for (auto it=metaRambo[b + B*r].begin(); it != metaRambo[b + B*r].end(); ++it)
          {std::cout << " " << *it;}
        {std::cout << "////";}
      }
      std::cout << '\n';
    }
}
}

// give set and keys in the set
void RAMBO::insertion (std::string setID, std::vector<std::string> keys){
  vector<uint> hashvals = RAMBO::hashfunc(setID, setID.size()); // R hashvals
  //make this loop parallel
  #pragma omp parallel for
  for(std::size_t i=0; i<keys.size(); ++i){
    for(int r=0; r<R; r++){
      vector<uint> temp = myhash(keys[i].c_str(), keys[i].size() , k, r, range);
      int loc = (hashvals[r] + B*r)*range/8;
      for (int n =0 ; n<temp.size(); n++){
        Rambo_array[loc + (temp[n]/8)] |= (1 << (temp[n]%8));
      }
      // Rambo_array[hashvals[r] + B*r]->insert(temp);
    }
  }
}

// given inverted index type arrangement, kmer;files;files;..
void RAMBO::insertion2 (std::vector<string> alllines){
  #pragma omp parallel for
  for(std::size_t i=0; i<alllines.size(); ++i){
    char d = ';';
    std::vector<string>KeySets =  line2array(alllines[i], d);//sets for a key

    std::vector<string>KeySet = line2array(KeySets[1], ',');
    for (uint j = 0; j<KeySet.size(); j++){
      vector<uint> hashvals = RAMBO::hashfunc(KeySet[j], KeySet[j].size()); // R hashvals
      for(int r=0; r<R; r++){
        vector<uint> temp = myhash(KeySets[0].c_str(), KeySets[0].size() , k, r, range);// need optimization here

        //
        int loc = (hashvals[r] + B*r)*range/8;
        for (int n =0 ; n<temp.size(); n++){
          Rambo_array[loc + (temp[n]/8)] |= (1 << (temp[n]%8));
        }
        // Rambo_array[hashvals[r] + B*r]->insert(temp);
      }
    }
  }
}

bitArray RAMBO::query (string query_key, int len){
  // set<int> resUnion[R]; //constains union results in it.
  bitArray bitarray_K(Ki);
  // bitset<Ki> bitarray_K;
  // set<int> res;

  for(int r=0; r<R; r++){
    chrono::time_point<chrono::high_resolution_clock> t1 = chrono::high_resolution_clock::now();
    vector<uint> check = myhash(query_key, len , k, r, range); //hash values correspondign to the keys
    chrono::time_point<chrono::high_resolution_clock> t2 = chrono::high_resolution_clock::now();
    // cout<<"time for hash calc: "<<((t2-t1).count()/1000000000.0)<<endl;;

    bitArray bitarray_K1(Ki);
    // bitset<Ki> bitarray_K1;
    float count=0.0;
    // #pragma omp parallel for
    chrono::time_point<chrono::high_resolution_clock> t3 = chrono::high_resolution_clock::now();

    for(int b=0; b<B; b++){
        bool MTcheck = true;
        for (int n =0 ; n<check.size(); n++){
            if (!(Rambo_array[(b + B*r)*range/8 + (check[n]/8)] & (1 << (check[n]%8)))){
              MTcheck = false;
              break;
            }
          }
        //Rambo_array[b + B*r]->test(check)
        if (MTcheck){
          for (uint j=0; j<metaRambo[b + B*r].size(); j++){
            bitarray_K1.SetBit(metaRambo[b + B*r][j]);
        }
        // count+=((t6-t5).count()/1000000000.0);
      }
    }
    chrono::time_point<chrono::high_resolution_clock> t4 = chrono::high_resolution_clock::now();
    // cout<<"time for bit setting: "<<((t4-t3).count()/1000000000.0)<<endl;
    count+=((t4-t3).count()/1.0);

    // cout<<"time for BF check : "<<count/B<<endl;
    if (r ==0){
      bitarray_K = bitarray_K1;
    }
    else{
      chrono::time_point<chrono::high_resolution_clock> t5 = chrono::high_resolution_clock::now();
      bitarray_K.ANDop(bitarray_K1.A);
      chrono::time_point<chrono::high_resolution_clock> t6 = chrono::high_resolution_clock::now();
      // cout<<"time for AND op : "<<((t6-t5).count()/1000000000.0)<<endl;;
    }
  }
  return bitarray_K;
}


void RAMBO::serializeRAMBO(string dir){
  //todo
  string br = dir + to_string(B) + "_" + to_string(R) + "_" + to_string(range)+ ".txt";
  ofstream out;
  out.open(br);

  if(! out){
    cout<<"Cannot open output file\n";
  }
  out.write(Rambo_array, B*R*range/8 +1);
    out.close();
}

void RAMBO::deserializeRAMBO(vector<string> dir){

  string br = dir[0] + to_string(B) + "_" + to_string(R) + "_" + to_string(range)+ ".txt";
  ifstream in(br);
  if(! in){
    cout<<"Cannot open searilized file\n";
  }
  in.read(Rambo_array, B*R*range/8 +1);
  in.close();
}
