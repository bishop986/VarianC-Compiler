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

	////// test hash table
	/*
	dh::symNodePtr tmp_node(new dh::symTabElem);
	tmp_node->name = "f";
	::std::shared_ptr< dh::symTab > tab(new dh::symTab("f",1));
	tab->insert_elem(tmp_node);

	::std::cout << "[A0] " << tab->get_in_function() << ::std::endl;
	::std::cout << "[A] " << (tab->research_elem(tab->get_in_function()) == nullptr) << ::std::endl;

	::std::shared_ptr< dh::symTab > tab2(new dh::symTab("f",1));

	tab2->set_upper_tab(tab);

	::std::cout << "[A] " << (tab2->research_elem(tab->get_in_function()) == nullptr) << ::std::endl;
	::std::cout << "[B] " << (tab2->research_elem_global(tab->get_in_function()) == nullptr) << ::std::endl;
	*/
	return 0;
	
}
