/*
 * Author: Dh
 * Update date: 2018.05.22
 * update info:
 * init parser
 */

#ifndef _ANALYSIS_H_
#define _ANALYSIS_H_

#include "scan.h"
#include "global.h"
#include "syntax_treenode.h"
#include "symbol_table.h"
#include <map>
#include <sstream>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

namespace dh{

class get_visitor : public ::boost::static_visitor<::std::string>{
	public:
		::std::string operator()(::std::string& s) const
		{
			return s;
		}

		::std::string operator()(int& num) const
		{
			::std::ostringstream ss;

			ss << num;
			return ss.str();
		}

		::std::string operator()(float& num) const
		{
			::std::ostringstream ss;

			ss << num;
			return ss.str();
		}
};

struct trivalitem
{
	trivalitem()
	{
		this->name = "";
		this->type = -100;
	}
	trivalitem(const ::std::string& name, const int& type)
	{
		this->name = name;
		this->type = type;
	}
	::std::string name;
	int type;
};

class trival
{
	public:
		trival( ::std::string operand)
		{
			this->_operand = operand;
			size = 0;
		}

		trival( ::std::string operand, trivalitem val1)
		{
			this->_operand = operand;
			this->_val1.name = val1.name;
			this->_val1.type = val1.type;
			size = 1;
		}

		trival( trivalitem res, ::std::string operand, trivalitem val1)
		{
			this->_res.name = res.name;
			this->_res.type = res.type;
			this->_operand = operand;
			this->_val1.name = val1.name;
			this->_val1.type = val1.type;
			size = 2;
		}

		trival( trivalitem res, 
				::std::string operand, 
				trivalitem val1, 
				trivalitem val2)
		{
			this->_res.name = res.name;
			this->_res.type = res.type;
			this->_operand = operand;
			this->_val1.name = val1.name;
			this->_val1.type = val1.type;
			this->_val2.name = val2.name;
			this->_val2.type = val2.type;
			size = 3;
		}

		trivalitem getRes() const
		{
			return _res;
		}

		::std::string getOperand() const
		{
			return _operand;
		}

		trivalitem getVal1() const
		{
			return _val1;
		}

		trivalitem getVal2() const
		{
			return _val2;
		}

		int getSize() const
		{
			return size;
		}

	private:
		trivalitem _res;
		::std::string _operand;
		trivalitem _val1;
		trivalitem _val2;
		int size;
};

class analysis{
	public:

		analysis( scanner tokens);
		NodePtr getRoot();
		void printTree() ;
		void initSynTree();
		//void genMidCode();
		//void exportMidCode( const ::std::string path); 
	private:

		int layer_record;
		void printTree( const NodePtr& ptr) ;
		bool initFlag;

		// compute attr of node and insert symbol to table

		// current area symbol table
		::std::shared_ptr<symTab> current_tab;

		//int getResult( const NodePtr& root);
		void match( ::std::string c);

		int numberInteger();

		float numberFloat();

		::std::string identifier();

		scanner _tokens;
		::std::shared_ptr< token > tmp;
		NodePtr _root;

		// recursion analysis
		NodePtr program();
		NodePtr func_decl();
		NodePtr var_decl();
		NodePtr init_declarator_list();
		NodePtr init_declarator();
		NodePtr declarator();
		NodePtr initialiser();
		NodePtr compound_stmt();
		NodePtr stmt();
		NodePtr if_stmt();
		NodePtr for_stmt();
		NodePtr while_stmt();
		NodePtr break_stmt();
		NodePtr continue_stmt();
		NodePtr return_stmt();
		NodePtr expr_stmt();
		NodePtr expr();
		NodePtr assignment_expr();
		NodePtr cond_or_expr();
		NodePtr cond_and_expr();
		NodePtr equality_expr();
		NodePtr rel_expr();
		NodePtr additive_expr();
		NodePtr multiplicative_expr();
		NodePtr unary_expr();
		NodePtr primary_expr();
		NodePtr para_list();
		NodePtr proper_para_list();
		NodePtr para_decl();
		NodePtr arg_list();
		NodePtr proper_arg_list();
		NodePtr arg();
		
		// semantics analyze
		int tmp_dType;
		int current_stmt;

		// gen midcode:
		int tmp_name_counter;
		int tmp_lable_counter;
		::std::string newtmpVal();
		::std::string newtmpLab();
		void genMidCode( const NodePtr& ptr, 
				const ::std::string& lable1, 
				const ::std::string& lable2);
		bool genFlag;

		::std::vector<trival> midcodes;
		void evalType( const NodePtr& ptr);

		friend class generator;

};

}

#endif
