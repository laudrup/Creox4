/***************************************************************************
					crdatastream.cpp  -  description
							 -------------------
		begin                : Sun May 20 2001
		copyright            : (C) 2001 by Jozef Kosoru
                                     : (C) 2011 by Kasper Laudrup
		email                : jozef.kosoru@pobox.sk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 *                                                                         *
 ***************************************************************************/

#include "control.h"
#include <cassert>
#include <iostream>
#include "crdatastream.h"

#if 0
void test()
{
	char* array = new char[100];
	CrInputDataStream stream(array, 100);
	const int x = 120;
	stream << 10 << 10.6f << 12.7 << x;

	CrOutputDataStream outStream(array, 2*sizeof(int)+sizeof(float)+sizeof(double));
//  CrOutputDataStream outStream(stream);
	int i1, i2;
	float f1;
	double d1;
	outStream >> i1 >> f1 >> d1 >> i2;
	if(outStream.eof()) std::cerr << "eof\n";
	std::cerr << i1 << f1 << d1 << i2 << "\n";
}
#endif

