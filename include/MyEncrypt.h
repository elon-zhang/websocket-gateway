#ifndef  ENCRYTP_H_
#define  ENCRYTP_H_
#include "stdio.h"
#include "stdlib.h"
#include <string.h>
#include <unistd.h>
#include <map>
#include <iostream>

using namespace std;

class MyEncrypt
{
public:
	MyEncrypt();

	~MyEncrypt();

	int getencrypt(const string & str, string & enstr);
private:
	
	int HmacEncode(const char * algo, const char * key, unsigned int key_length, const char * input, unsigned int input_length, unsigned char * &output, unsigned int &output_length);
};
#endif
