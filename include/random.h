// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the RANDOM_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// RANDOM_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifndef RANDOM_EXPORTS
#define RANDOM_EXPORTS 

#include <vector>
// This class is exported from the random.dll
using namespace std;

 typedef unsigned int uint32_t;
 typedef  std::vector<uint32_t>  Int_Array;

#define MAX_N	120

 namespace RandomApi{


   void  Get_1_Deck_Card_WJ(uint32_t n,  Int_Array &IntOutArr); // 先隨機0~54,然後再抽n張牌組成數組輸出; 龍虎;

   void  Get_1_Deck_Card(uint32_t n, Int_Array &IntOutArr); // 先隨機0~52,然後再抽n張牌組成數組輸出; 扑克


}

#endif
