#include "MurmurHash3.h"
#include <iostream>
#include <cstring>
#include <chrono>
#include <vector>
#include "MyBloom.h"
#include <math.h>
#include "constants.h"
#include <bitset>
#include "bitArray.h"

using namespace std;

vector<unsigned int> myhash( std::string key, int len, int k, int r, int range){
  // int hashvals[k];
  vector <unsigned int> hashvals;
  unsigned int op; // takes 4 byte

  for (int i=0+ k*r; i<k+ k*r; i++){
    MurmurHash3_x86_32(key.c_str(), len, i, &op);
    hashvals.push_back(op%range);
  }
  return hashvals;
}

BloomFiler::BloomFiler(int sz, float FPR, int k){
      p = FPR;
      k = k; //number of hash
      m_bits = new bitArray(sz);
      }

void BloomFiler::insert(vector<unsigned int> a){
  int N = a.size();
  for (int n =0 ; n<N; n++){
    m_bits->SetBit(a[n]);
  }
}

bool BloomFiler::test(vector<unsigned int> a){
  int N = a.size();
  for (int n =0 ; n<N; n++){
      if (!m_bits->TestBit(a[n])){
        return false;
      }
  }
  return true;
}

void BloomFiler::serializeBF(string BF_file){
  m_bits->serializeBitAr(BF_file);
}

void BloomFiler::deserializeBF(vector<string> BF_file){
  m_bits->deserializeBitAr(BF_file);
}
