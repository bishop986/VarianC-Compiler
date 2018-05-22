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

class trival
{
	public:
		trival( ::std::string operand, ::std::string val1)
		{
			this->_operand = operand;
			this->_val1 = val1;
			size = 1;
		}

		trival( ::std::string res, ::std::string operand, ::std::string val1)
		{
			this->_res = res;
			this->_operand = operand;
			this->_val1 = val1;
			size = 2;
		}

		trival( ::std::string res, 
				::std::string operand, 
				::std::string val1, 
				::std::string val2)
		{
			this->_res = res;
			this->_operand = operand;
			this->_val1 = val1;
			this->_val2 = val2;
			size = 3;
		}

		::std::string getRes() const
		{
			return _res;
		}

		::std::string getOperand() const
		{
			return _operand;
		}

		::std::string getVal1() const
		{
			return _val1;
		}

		::std::string getVal2() const
		{
			return _val2;
		}

		int getSize() const
		{
			return size;
		}

	private:
		::std::string _res;
		::std::string _operand;
		::std::string _val1;
		::std::string _val2;
		int size;
};

class analysis{
	public:

		analysis( scanner tokens);
		auto getRoot();
		//void buildSymTab();
		//void genMidCode();
		//void exportMidCode( const ::std::string path); 
	private:

		void printTree( const NodePtr& ptr) const;
		void initSynTree();
		bool initFlag;

		// compute attr of node and insert symbol to table
		//void EvalType( const NodePtr& ptr);

		// build symbol table
		//void buildSymTab( const NodePtr& ptr);

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
		NodePtr decl();
		NodePtr init_declarator_list();
		NodePtr init_declarator();
		NodePtr delclarator();
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
		NodePtr equlity_expr();
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

		// gen midcode:
		/*
		int tmp_name_counter;
		::std::string newtmpVal();
		::std::string newtmpLab();
		void genMidCode( const NodePtr& ptr);
		bool genFlag;

		::std::map< ::std::string, int> symTab;
		::std::vector<trival> midcodes;
		*/

		friend class generator;

};

}

#endif
