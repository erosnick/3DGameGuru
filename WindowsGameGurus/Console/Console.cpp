// Console.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <MMSystem.h>
#include <vector>
#include <xnamath.h>

using namespace std;

#define _RGB16BIT565(r, g, b) ((b)) + ((g) << 5) + ((r) << 11)
#define  RGB32BIT(a, r, g, b) ((b) + (g << 8) + (r << 16) + (a << 24))

USHORT RGB16BIT565(int r, int g, int b)
{
	r >>= 3;
	g >>= 2;
	b >>= 3;

	return (_RGB16BIT565(r, g, b));
}

class A
{
};

int _tmain(int argc, _TCHAR* argv[])
{
	vector<A*> v(0);

	return 0;
}

