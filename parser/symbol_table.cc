#include "../include/symbol_table.h"
#include <iostream>
namespace dh{

symTab::symTab( ::std::string in_function)
{
	this->container.resize(256,nullptr);
	this->upper_tab = nullptr;
	this->in_function = in_function;
	this->totaloffset = 0;
}

void symTab::insert_elem(const symNodePtr& ptr)
{
	if ( ptr != nullptr)
	{
		if ( ptr->nametype != 0)
		{
			switch(ptr->datatype)
			{
				case TypeKind::IntK:
				case TypeKind::FloatK:
					totaloffset += 4;
					break;
				case TypeKind::IntArrayK:
				case TypeKind::FloatArrayK:
					totaloffset += ptr->arraylength * 4;
					break;
				case TypeKind::BoolK:
					totaloffset += 1;
					break;
				case TypeKind::BoolArrayK:
					totaloffset += ptr->arraylength * 1;
					break;
			}
			ptr->offset = totaloffset;
		}
		int pos = hash_pos(ptr->name);
		if ( container[pos] != nullptr)
		{
			symNodePtr tmp = container[pos];
			container[pos] = ptr;
			ptr->next = tmp;
		} else 
		{
			container[pos] = ptr;
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

::std::shared_ptr<symTab> symTab::get_upptr_tab()
{
	return this->upper_tab;
}

void symTab::append_down_tab( const ::std::shared_ptr<symTab>& ptr)
{
	this->donw_tabs.push_back(ptr);
}

::std::vector< ::std::shared_ptr<symTab> > symTab::get_down_tabs()
{
	return this->donw_tabs;
}

::std::string symTab::get_in_function() const
{
	return this->in_function;
}

int symTab::hash_pos(const ::std::string& c)
{
	int tmp = 0;
	for ( auto i : c)
	{
		tmp = (( tmp << 2) + i) % 256;
	}
	return tmp;
}
}
