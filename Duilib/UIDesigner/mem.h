#ifndef _MEM_H_
#define _MEM_H_

#pragma once

#include <map>
#include <utility>

using namespace std;
#pragma warning(disable:4291)

#define DIM(A) (sizeof(A)/sizeof(A[0]))

typedef struct tag_MemoryInfo
{
	char tszFile[MAX_PATH];
	ULONG cchLine;
	size_t nMemSize;
}MemoryInfo;


extern map<DWORD,MemoryInfo> g_MemList;
void* __cdecl operator new(size_t nSize,LPCSTR lpszFileName,int nLine);
void __cdecl operator delete(void *pvMem);

void* __cdecl operator new[](size_t nSize, LPCSTR lpszFileName, int nLine);
void __cdecl operator delete[](void *);

#define DEBUG_NEW2 new(__FILE__, __LINE__)
#define DEBUG_DELETE2 delete(__FILE__, __LINE__)

#endif // _MEM_H_