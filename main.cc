/*
 * Author: Dh
 * Update Date: 2018.5.21
 * Update info: integrate word scanner
 */

#include <iostream>
#include <cstdio>

#include "include/global.h"
#include "include/scan.h"
#include "include/syntax_treenode.h"
#include "include/analysis.h"

// compiler enter

int main( int argc, char** argv)
{
	if( argc <= 1)
	{
		::std::printf("[HINT] Usage: vcc {<option> <args>} <filename>.c\n");
		::std::exit(1);
	}

	FILE* fp = ::std::fopen(argv[1],"r");
	if ( NULL == fp)
	{
		::std::perror(argv[1]);
		::std::exit(1);
	}

	::dh::scanner tokens;
	
	tokens.scan(fp);

	if ( !tokens.isRight())
	{
		return 1;
	}

#ifdef _DEBUG_
	tokens.debug();
#endif

	::dh::analysis an(tokens);
	an.initSynTree();
	an.printTree();
	return 0;
	
}
