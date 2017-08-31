//============================================================================
// Name        : comn.cpp
// Author      : chuanjiang.zh@qq.com
// Version     :
// Copyright   : all right reserved.
// Description : Hello World in C, Ansi-style
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "CStr.h"
#include "OptionParser.h"

using namespace comn;



int main(int argc, char** argv) {
	puts("!!!Hello World!!!");

	{
	    /// empty
	    CStr str;
	}

	{
	    const char* text = "abc";
	    CStr str(text);
	    str.size();
	}

	{
	    char buffer[8];
	    CStr str(buffer, sizeof(buffer));
	    str = "hello";
	    str = "0123456789";
	}


	OptionParser parser("%prog", "demo 1.0");
	parser.enableHelp();
	parser.enableVersion();
	parser.addOption('u', "user", true, "", "username");
	parser.addOption('p', "password", false, "", "password");

	if (!parser.parse(argc, argv))
	{
		parser.usage();
	}

	for (size_t i = 0; i < parser.getArgCount(); i ++)
	{
		std::cout << parser.getArg(i) << std::endl;
	}

    std::string user;
    parser.getOption("user", user);

	return EXIT_SUCCESS;
}
