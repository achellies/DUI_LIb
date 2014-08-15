#include "stdafx.h"
#include "mem.h"

map<DWORD,MemoryInfo> g_MemList;
void* __cdecl operator new(size_t nSize,LPCSTR lpszFileName,int nLine)
{
	PVOID pVoid = ::operator new(nSize);
	MemoryInfo mem;
	mem.cchLine = nLine;
	mem.nMemSize = nSize;
	strcpy_s(mem.tszFile,DIM(mem.tszFile),lpszFileName);
	g_MemList[(DWORD)pVoid] = mem;
	return pVoid;
}

void __cdecl operator delete(void* p)
{
	DWORD nKeyword = (DWORD)p;
	if (!g_MemList.empty())
	{
		map<DWORD,MemoryInfo>::iterator iter= g_MemList.find(nKeyword);
		if (iter != g_MemList.end())
		{
			g_MemList.erase(iter);
		}
	}
	::free(p);
}

void* __cdecl operator new[](size_t nSize, LPCSTR lpszFileName, int nLine)
{
	PVOID pVoid = ::operator new(nSize);
	MemoryInfo mem;
	mem.cchLine = nLine;
	mem.nMemSize = nSize;
	strcpy_s(mem.tszFile,DIM(mem.tszFile),lpszFileName);
	g_MemList[(DWORD)pVoid] = mem;
	return pVoid;
}

void __cdecl operator delete[](void *p)
{
	DWORD nKeyword = (DWORD)p;
	if (!g_MemList.empty())
	{
		map<DWORD,MemoryInfo>::iterator iter= g_MemList.find(nKeyword);
		if (iter != g_MemList.end())
		{
			g_MemList.erase(iter);
		}
	}
	::free(p);
}