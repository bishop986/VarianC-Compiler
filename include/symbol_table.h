#include "./global.h"
#include "./syntax_treenode.h"
#include <vector>
#include <memory>

namespace dh{

struct symTabElem;

typedef ::std::shared_ptr<symTabElem> symNodePtr;

struct symTabElem
{
	bool isVariable;
	int type;
	::std::string name;
	::std::vector<int> args;

	symNodePtr next;
};

class symTab
{
	public:
		symTab();
		void insert_elem(const symNodePtr& ptr);
		void delete_elem(const symNodePtr& ptr);
		void set_upper_tab(const::std::shared_ptr<symTab>& upper);
		symNodePtr research_elem(const ::std::string& c);
		symNodePtr research_elem_global(const ::std::string& c);
	private:
		::std::shared_ptr<symTab> upper_tab;
		::std::vector<symNodePtr> container;
		int hash_pos( const ::std::string& c);
};

}
