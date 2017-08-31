/*    file: main.cpp
 *    desc:
 * 
 * created: 2016-04-06
 *  author: chuanjiang.zh@qq.com
 * company: 
 */

#include <stdio.h>
#include <iostream>

#include "Application.h"


int main(int argc, char** argv)
{
	Application app;

	if (!app.initialize())
	{
		return EXIT_FAILURE;
	}

	app.run();

	return 0;
}


