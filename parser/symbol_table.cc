#include "../include/symbol_table.h"

namespace dh{

symTab::symTab()
{
	this->container.resize(256,nullptr);
	this->upper_tab = nullptr;
}

void symTab::insert_elem(const symNodePtr& ptr)
{
	if ( ptr != nullptr)
	{
		int pos = hash_pos(ptr->name);
		if ( container[pos] != nullptr)
		{
			symNodePtr tmp = container[pos];
			container[pos] = ptr;
			ptr->next = tmp;
		}
	}
}

void symTab::delete_elem(const symNodePtr& ptr)
{
	if ( ptr != nullptr)
	{
		int pos = hash_pos(ptr->name);
		if ( container[pos] != nullptr)
		{
			symNodePtr tmp = container[pos]->next;
			container[pos] = tmp;
		}
	}
}

symNodePtr symTab::research_elem( const ::std::string& c)
{
	int pos = hash_pos(c);
	if ( container[pos] != nullptr)
	{
		symNodePtr tmp_cur_ptr = container[pos];
		while (1)
		{
			if ( tmp_cur_ptr  == nullptr)
			{
				break;
			}
			if ( tmp_cur_ptr->name == c)
			{
				return tmp_cur_ptr;
			}
			tmp_cur_ptr = tmp_cur_ptr->next;
		}
	}
	return nullptr;
}

symNodePtr symTab::research_elem_global( const ::std::string& c)
{
	auto tmp_ptr = research_elem(c);
	if ( tmp_ptr == nullptr)
	{
		if ( this->upper_tab == nullptr)
		{
			return nullptr;
		}
		return this->upper_tab->research_elem_global(c);
	} else 
	{
		return tmp_ptr;
	}
}

void symTab::set_upper_tab( const ::std::shared_ptr<symTab>& upper)
{
	this->upper_tab = upper;
}
}
