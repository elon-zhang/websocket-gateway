#pragma once

#include <map>
#include "string.h"

typedef unsigned char       BYTE;

using namespace std;

struct tagWin
{
	BYTE size;
	BYTE type;
	BYTE data[20];

	BYTE seatId;
	BYTE role;

	BYTE size2;
	BYTE type2;
	BYTE data2[20];

	tagWin()
	{
		size = 0;
		type = 0;
		memset(data, 0, 20);

		seatId = -1;
		role = -1;

		size2 = 0;
		type2 = 0;
		memset(data2, 0, 20);
	}
};

class CMultiSequenceAnalysis
{
public:
	CMultiSequenceAnalysis();
	~CMultiSequenceAnalysis();

	void Add(int score, tagWin* tw);
	tagWin * GetBestItem();

	int WinOrLose(int &w, int &l);

	tagWin * GetBestItem(CMultiSequenceAnalysis *msa);

	short GetBestWinLlose(CMultiSequenceAnalysis * msa);

	int GetMaxScore();
private:
	typedef multimap<int, tagWin> mmap_t;
	typedef multimap <int, tagWin>::const_reverse_iterator rit;
	typedef multimap <int, tagWin>::iterator cit;
	typedef pair<cit, cit> Range;

	mmap_t mmap;
};

