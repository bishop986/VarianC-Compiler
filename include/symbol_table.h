#ifndef _SYMBOL_TABLE_H_
#define _SYMBOL_TABLE_H_

#include "./global.h"
#include "./syntax_treenode.h"
#include <vector>
#include <memory>

namespace dh{

struct symTabElem;

typedef ::std::shared_ptr<symTabElem> symNodePtr;

struct symTabElem
{
	char nametype;
	int datatype;
	::std::string name;
	::std::vector<int> args;

	symNodePtr next;
};

class symTab
{
	public:
		symTab(::std::string in_function);
		void insert_elem(const symNodePtr& ptr);
		void delete_elem(const symNodePtr& ptr);
		void set_upper_tab(const ::std::shared_ptr<symTab>& upper);
		void append_down_tab(const ::std::shared_ptr<symTab>& upper);
		::std::shared_ptr<symTab> get_upptr_tab();
		::std::vector< ::std::shared_ptr<symTab> > get_down_tabs();
		::std::string get_in_function() const;
		symNodePtr research_elem(const ::std::string& c);
		symNodePtr research_elem_global(const ::std::string& c);
	private:
		::std::shared_ptr<symTab> upper_tab;
		::std::vector< ::std::shared_ptr<symTab> > donw_tabs;
		::std::vector<symNodePtr> container;
		int hash_pos( const ::std::string& c);
		::std::string in_function;
};

}

#endif
