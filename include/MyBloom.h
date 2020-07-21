#ifndef _MYBLOOM_
#define _MYBLOOM_
#include <vector>
#include "constants.h"
#include <bitset>
#include "bitArray.h"

std::vector<unsigned int> myhash(std::string key, int len, int k, int r, int range);

class BloomFiler{
    public:
        // BloomFiler(int capacity, float FPR, int k);
        BloomFiler(int sz, float FPR, int k);
        void insert(std::vector<unsigned int> a);
        bool test(std::vector<unsigned int> a);
        void serializeBF(std::string BF_file);
        void deserializeBF(std::vector<std::string> BF_file);

        // void serialize1(std::string BF_file);

        int n;
        float p;
        int R;
        int k;
        // std::vector<bool> m_bits;
        // std::bitset<capacity> m_bits;
        bitArray* m_bits;
};

#endif
