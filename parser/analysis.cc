/*
 * Author: Dh
 * Update Date: 2018.5.29
 * Update info: analysis word sequence
 */
#include "../include/analysis.h"
#include "../include/global.h"
#include "../include/scan.h"
#include <sstream>

namespace dh{

analysis::analysis( scanner tokens)
{
	this->layer_record = 0;
	this->_tokens = tokens;
	this->_root = nullptr;
	this->tmp = ::std::make_shared<token>("", TYPE::NONES, 0);
	initFlag = false;

#ifdef _DEBUG_
	assert(_tokens.isScanned());
#endif

	if ( !tokens.isScanned())
	{
		::std::cout << "[ERROR] Token Squence\n";
		::std::exit(1);
	}
}

void analysis::initSynTree()
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG] in init\n";
#endif
	*tmp = _tokens.getToken();
	this->_root = program();
	initFlag = true;
}

NodePtr analysis::getRoot()
{
	return _root;
}

void analysis::match( ::std::string c)
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG IN Match] get " 
		<< tmp->getVal() 
		<< " need " 
		<< c 
		<< ::std::endl;
#endif
	if ( c == tmp->getVal())
	{
		*tmp = _tokens.getToken();
		return ;
	} else 
	{
		::std::cerr << "[ERROR] Incompitable word: \""
			<< tmp->getVal() 
			<< "\" in line "
			<< tmp->getLineno()
			<< " expect token: \""
			<< c
			<< "\"\n";
		::std::exit(1);
	}
}

int analysis::numberInteger()
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG] in numberInteger\n";
#endif

	if ( tmp->getType() == TYPE::NUMINT)
	{
		::std::stringstream ss;
		ss << tmp->getVal();

		int ret;
		ss >> ret;
		*tmp = _tokens.getToken();

		return ret;
	} else 
	{
		::std::cerr << "[ERROR] Incompitable type: \""
			<< tmp->getType()
			<< "\" in line "
			<< tmp->getLineno()
			<< " expect int type\n";
		::std::exit(1);
	}
}

float analysis::numberFloat()
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG] in numberFloat\n";
#endif

	if ( tmp->getType() == TYPE::NUMFLOAT)
	{
		float ret;
		::std::stringstream ss;
		ss << tmp->getVal();
		ss >> ret;

		*tmp = _tokens.getToken();

		return ret;
	} else 
	{
		::std::cerr << "[ERROR] Incompitable type: \""
			<< tmp->getVal()
			<< "\" in line "
			<< tmp->getLineno()
			<< " expect float type\n";
		::std::exit(1);
	}
}

::std::string analysis::identifier()
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG] in identifier\n";
#endif 

	if ( tmp->getType() == TYPE::ID)
	{
		::std::string ret = tmp->getVal();
		*tmp = _tokens.getToken();
		return ret;
	} else 
	{
		::std::cerr << "[ERROR] Incompitable type: \""
			<< tmp->getVal()
			<< "\" in line "
			<< tmp->getLineno()
			<< " expect ID type\n";
		::std::exit(1);
	}
}

NodePtr analysis::program()
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG] in program\n";
#endif
	NodePtr ret = nullptr;

	if ( tmp->getType() == TYPE::VOID 
			|| tmp->getType() == TYPE::INT
			|| tmp->getType() == TYPE::FLOAT
			|| tmp->getType() == TYPE::BOOLEAN)
	{

		// do pre-assign match
		::std::string tmp_type = tmp->getVal();
		match(tmp->getVal());
		::std::string tmp_id = identifier();

		if ( tmp->getVal() == ";" || tmp->getVal() == "["
				|| tmp->getVal() == "," || tmp->getVal() == "=")
		{
			_tokens.ungetToken();
			_tokens.ungetToken();
			_tokens.ungetToken();
			*tmp = _tokens.getToken();
			ret = var_decl();
		} else 
		{
			_tokens.ungetToken();
			_tokens.ungetToken();
			_tokens.ungetToken();
			*tmp = _tokens.getToken();
			ret = func_decl();
		}
	}

	NodePtr tmp_cur_ptr = ret;
	while( tmp->getType() == TYPE::VOID 
			|| tmp->getType() == TYPE::INT
			|| tmp->getType() == TYPE::FLOAT
			|| tmp->getType() == TYPE::BOOLEAN)
	{
		NodePtr tmp_in_ptr;

		::std::string tmp_type = tmp->getVal();
		match(tmp->getVal());
		::std::string tmp_id = identifier();

		if ( tmp->getVal() == ";" || tmp->getVal() == "["
				|| tmp->getVal() == "," || tmp->getVal() == "=")
		{
			_tokens.ungetToken();
			_tokens.ungetToken();
			_tokens.ungetToken();
			*tmp = _tokens.getToken();
			tmp_in_ptr = var_decl();
		} else 
		{
			_tokens.ungetToken();
			_tokens.ungetToken();
			_tokens.ungetToken();
			*tmp = _tokens.getToken();
			tmp_in_ptr = func_decl();
		}

		tmp_cur_ptr->setSibling( tmp_in_ptr);
		tmp_cur_ptr = tmp_in_ptr;
	}

	return ret;
}

NodePtr analysis::func_decl()
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG] in func_decl, get type " << tmp->getVal() << ::std::endl;
#endif
	
	NodePtr tmp_ptr = ::std::make_shared<TreeNode>(tmp->getVal());
	match(tmp->getVal());

	::std::string tmp_str = identifier();
	NodePtr ret = ::std::make_shared<TreeNode>(tmp_str);
	ret->setNodeKind(NodeKind::DeclK);
	ret->setKind(DeclKind::FuncK);
	ret->appendChild(tmp_ptr);

	tmp_ptr = para_list();
	ret->appendChild(tmp_ptr);

	this->layer_record++;
	tmp_ptr = compound_stmt();
	this->layer_record--;

	ret->appendChild(tmp_ptr);
	return ret;
}

NodePtr analysis::var_decl()
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG] in var_decl\n";
#endif
	NodePtr ret = ::std::make_shared<TreeNode>(tmp->getVal());
	match(tmp->getVal());
	ret->setNodeKind(NodeKind::DeclK);
	ret->setKind(DeclKind::VarK);

	NodePtr tmp_ptr = init_declarator_list();
	ret->appendChild(tmp_ptr);

	match(";");

	return ret;
}

NodePtr analysis::init_declarator_list()
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG] in init_declarator_list\n";
#endif
	NodePtr ret = init_declarator();

	NodePtr tmp_cur_ptr = ret;
	while ( tmp->getVal() == ",")
	{
		match(",");
		NodePtr tmp_in_ptr = init_declarator();
		tmp_cur_ptr->setSibling(tmp_in_ptr);
		tmp_cur_ptr = tmp_in_ptr;
	}

	return ret;
}

NodePtr analysis::init_declarator()
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG] in init_declarator\n";
#endif
	NodePtr ret = declarator();
	NodePtr tmp_ptr;

	if ( tmp->getVal() == "=")
	{
		match("=");
		tmp_ptr = initialiser();
		ret->appendChild(tmp_ptr);
	}

	return ret;
}

NodePtr analysis::declarator()
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG] in declarator\n";
#endif
	::std::string tmp_str = identifier();
	NodePtr ret = ::std::make_shared<TreeNode>(tmp_str);
	ret->setNodeKind(NodeKind::DeclaratorK);

	if ( tmp->getVal() == "[")
	{
		match("[");
		int tmp_int = numberInteger();
		NodePtr tmp_ptr = ::std::make_shared<TreeNode>(tmp_int);
		tmp_ptr->setNodeKind(NodeKind::ExprK);
		tmp_ptr->setKind(ExprKind::INTLITERAL);
		tmp_ptr->setType(TypeKind::IntK);
		match("]");
		ret->setKind(DeclaratorKind::ArrayK);
		ret->appendChild(tmp_ptr);
	} else 
	{
		ret->setKind(DeclaratorKind::PrimitiveK);
	}

	return ret;
}

NodePtr analysis::initialiser()
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG] in initialiser\n";
#endif

	NodePtr ret = ::std::make_shared<TreeNode>("=");
	ret->setNodeKind(NodeKind::InitialiserK);
	NodePtr tmp_ptr;

	if ( tmp->getVal() == "{")
	{
		ret->setKind(InitialiserKind::ListK);
		match("{");
		tmp_ptr = expr();
		ret->setSibling(tmp_ptr);

		NodePtr tmp_cur_ptr = tmp_ptr;
		while(tmp->getVal() != ",")
		{
			NodePtr tmp_in_ptr = expr();
			tmp_cur_ptr->setSibling(tmp_in_ptr);
			tmp_cur_ptr = tmp_in_ptr;
		}
		match("}");
	} else 
	{
		ret->setKind(InitialiserKind::SingleK);
		tmp_ptr = expr();
	}
	ret->appendChild(tmp_ptr);
	return ret;
}

NodePtr analysis::compound_stmt()
{
#ifdef _DEBGU_
	::std::cout << "[DEBUG] in compound_stmt \n";
#endif
	NodePtr ret = ::std::make_shared<TreeNode>("{}");
	NodePtr tmp_ptr = nullptr;
	ret->setNodeKind(NodeKind::StmtK);
	ret->setKind(StmtKind::ComK);

	match("{");

	if ( tmp->getType() == TYPE::INT
			|| tmp->getType() == TYPE::VOID
			|| tmp->getType() == TYPE::FLOAT
			|| tmp->getType() == TYPE::BOOLEAN)
	{
		tmp_ptr = var_decl();
	}

	NodePtr tmp_cur_ptr = tmp_ptr;
	while(tmp->getType() == TYPE::INT
			|| tmp->getType() == TYPE::VOID
			|| tmp->getType() == TYPE::FLOAT
			|| tmp->getType() == TYPE::BOOLEAN)
	{
		NodePtr tmp_in_ptr = var_decl();
		tmp_cur_ptr->setSibling(tmp_in_ptr);
		tmp_cur_ptr = tmp_in_ptr;
	}
	ret->appendChild(tmp_ptr);

	tmp_ptr = nullptr;
	if ( tmp->getType() == TYPE::BREAK
			|| tmp->getType() == TYPE::RETURN
			|| tmp->getType() == TYPE::IF
			|| tmp->getType() == TYPE::CONTINUE
			|| tmp->getType() == TYPE::FOR
			|| tmp->getType() == TYPE::WHILE
			|| tmp->getType() == TYPE::ID
			|| tmp->getVal() == "{"
			|| tmp->getVal() == "(")
	{
		tmp_ptr = stmt();
	}

	tmp_cur_ptr = tmp_ptr;
	while ( tmp->getType() == TYPE::BREAK
			|| tmp->getType() == TYPE::RETURN
			|| tmp->getType() == TYPE::IF
			|| tmp->getType() == TYPE::CONTINUE
			|| tmp->getType() == TYPE::FOR
			|| tmp->getType() == TYPE::WHILE
			|| tmp->getType() == TYPE::ID
			|| tmp->getVal() == "{"
			|| tmp->getVal() == "(")
	{
		NodePtr tmp_in_ptr = stmt();
		tmp_cur_ptr->setSibling(tmp_in_ptr);
		tmp_cur_ptr = tmp_in_ptr;
	}
	ret->appendChild(tmp_ptr);

	match("}");

	return ret;
}

NodePtr analysis::stmt()
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG] in stmt\n";
#endif
	NodePtr ret;
	NodePtr tmp_ptr;

	if ( tmp->getVal() == "{")
	{
		layer_record ++;
		ret = compound_stmt();
		layer_record --;
	} else if ( tmp->getType() == TYPE::IF)
	{
		ret = if_stmt();
	} else if ( tmp->getType() == TYPE::FOR)
	{
		ret = for_stmt();
	} else if ( tmp->getType() == TYPE::WHILE)
	{
		ret = while_stmt();
	} else if ( tmp->getType() == TYPE::BREAK)
	{
		ret = break_stmt();
	} else if ( tmp->getType() == TYPE::CONTINUE)
	{
		ret = continue_stmt();
	} else if ( tmp->getType() == TYPE::RETURN)
	{
		ret = return_stmt();
	} else 
	{
		ret = expr_stmt();
	}

	return ret;
}

NodePtr analysis::if_stmt()
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG] in if_stmt\n";
#endif
	match("if");
	NodePtr ret = ::std::make_shared<TreeNode>("if");
	ret->setNodeKind(NodeKind::StmtK);
	ret->setKind(StmtKind::IfK);

	match("(");
	NodePtr tmp_ptr = expr();
	match(")");
	ret->appendChild(tmp_ptr);

	tmp_ptr = stmt();
	ret->appendChild(tmp_ptr);

	if ( tmp->getType() == TYPE::ELSE)
	{
		tmp_ptr = stmt();
		ret->appendChild(tmp_ptr);
	}

	return ret;
}

NodePtr analysis::for_stmt()
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG] in for_stmt\n";
#endif
	match("for");
	NodePtr ret= ::std::make_shared<TreeNode>("for");
	NodePtr tmp_ptr;

	ret->setNodeKind(NodeKind::StmtK);
	ret->setKind(StmtKind::ForK);

	match("(");

	if ( tmp->getVal() != ";")
	{
		tmp_ptr = expr();
		ret->appendChild(tmp_ptr);
	}
	match(";");

	if ( tmp->getVal() != ";")
	{
		tmp_ptr = expr();
		ret->appendChild(tmp_ptr);
	}
	match(";");

	if ( tmp->getVal() != ")")
	{
		tmp_ptr = expr();
		ret->appendChild(tmp_ptr);
	}
	match(")");

	tmp_ptr = stmt();
	ret->appendChild(tmp_ptr);

	return ret;
}

NodePtr analysis::while_stmt()
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG] in while_stmt\n";
#endif
	match("while");

	NodePtr ret = ::std::make_shared<TreeNode>("while");
	ret->setNodeKind(NodeKind::StmtK);
	ret->setKind(StmtKind::WhileK);

	match("(");
	NodePtr tmp_ptr = expr();
	match(")");
	ret->appendChild(tmp_ptr);

	tmp_ptr = stmt();
	ret->appendChild(tmp_ptr);

	return ret;
}

NodePtr analysis::break_stmt()
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG] in break_stmt\n";
#endif

	match("break");
	NodePtr ret = ::std::make_shared<TreeNode>("break");
	ret->setNodeKind(NodeKind::StmtK);
	ret->setKind(StmtKind::BreakK);
	match(";");

	return ret;
}

NodePtr analysis::continue_stmt()
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG] in continue_stmt\n";
#endif
	match("continue");
	NodePtr ret = ::std::make_shared<TreeNode>("continue");
	ret->setNodeKind(NodeKind::StmtK);
	ret->setKind(StmtKind::ContinueK);

	match(";");

	return ret;
}

NodePtr analysis::return_stmt()
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG] in return_stmt\n";
#endif
	match("return");
	NodePtr ret = ::std::make_shared<TreeNode>("return");
	ret->setNodeKind(NodeKind::StmtK);
	ret->setKind(StmtKind::RetK);

	if ( tmp->getType() != TYPE::SEPOP)
	{
		NodePtr tmp_ptr = expr();
		ret->appendChild(tmp_ptr);
	}
	match(";");

	return ret;
}

NodePtr analysis::expr_stmt()
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG] in expr_stmt\n";
#endif
	NodePtr ret = ::std::make_shared<TreeNode>("Expr");

	ret->setNodeKind(NodeKind::StmtK);
	ret->setKind(StmtKind::ExpK);

	if ( tmp->getType() != TYPE::SEPOP)
	{
		NodePtr tmp_ptr = expr();
		ret->appendChild(tmp_ptr);
	}
	match(";");

	return ret;
}

NodePtr analysis::expr()
{
#ifdef _DEBUG_
	::std::cout << "[DEBGU] in expr\n";
#endif
	NodePtr ret = assignment_expr();
	return ret;
}

NodePtr analysis::assignment_expr()
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG] in assignment_expr\n";
#endif
	
	NodePtr ret;
	NodePtr tmp_ptr = cond_or_expr();

	if ( tmp->getVal() == "=")
	{
		match("=");
		ret = ::std::make_shared<TreeNode>("=");
		ret->setNodeKind(NodeKind::ExprK);
		ret->setKind(ExprKind::AssignK);
		ret->appendChild(tmp_ptr);

		NodePtr tmp_cur_ptr = ret;
		while (1)
		{
			tmp_ptr = cond_or_expr();

			if ( tmp->getVal() == "=")
			{
				match("=");
				NodePtr tmp_in_ptr = ::std::make_shared<TreeNode>("=");
				tmp_in_ptr->setNodeKind(NodeKind::ExprK);
				tmp_in_ptr->setKind(ExprKind::AssignK);
				tmp_in_ptr->appendChild(tmp_ptr);

				tmp_cur_ptr->appendChild(tmp_in_ptr);
				tmp_cur_ptr = tmp_in_ptr;
			} else 
			{
				tmp_cur_ptr->appendChild(tmp_ptr);
				break;
			}
		}

	} else 
	{
		return tmp_ptr;
	}

	return ret;
}

NodePtr analysis::cond_or_expr()
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG] in cond_or_expr\n";
#endif
	
	NodePtr ret;
	NodePtr tmp_ptr = cond_and_expr();

	if ( tmp->getVal() == "||")
	{
		match("||");
		ret = ::std::make_shared<TreeNode>("||");
		ret->setNodeKind(NodeKind::ExprK);
		ret->setKind(ExprKind::CondOrK);
		ret->appendChild(tmp_ptr);

		NodePtr tmp_cur_ptr = ret;
		while (1)
		{
			tmp_ptr = cond_and_expr();

			if ( tmp->getVal() == "||")
			{
				match("||");
				NodePtr tmp_in_ptr = ::std::make_shared<TreeNode>("||");
				tmp_in_ptr->setNodeKind(NodeKind::ExprK);
				tmp_in_ptr->setKind(ExprKind::CondOrK);
				tmp_in_ptr->appendChild(tmp_ptr);

				tmp_cur_ptr->appendChild(tmp_in_ptr);
				tmp_cur_ptr = tmp_in_ptr;
			} else 
			{
				tmp_cur_ptr->appendChild(tmp_ptr);
				break;
			}
		}

	} else 
	{
		return tmp_ptr;
	}

	return ret;
}

NodePtr analysis::cond_and_expr()
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG] in cond_and_expr\n";
#endif
	
	NodePtr ret;
	NodePtr tmp_ptr = equality_expr();

	if ( tmp->getVal() == "&&")
	{
		match("&&");
		ret = ::std::make_shared<TreeNode>("&&");
		ret->setNodeKind(NodeKind::ExprK);
		ret->setKind(ExprKind::CondAndK);
		ret->appendChild(tmp_ptr);

		NodePtr tmp_cur_ptr = ret;
		while (1)
		{
			tmp_ptr = equality_expr();

			if ( tmp->getVal() == "&&")
			{
				match("&&");
				NodePtr tmp_in_ptr = ::std::make_shared<TreeNode>("&&");
				tmp_in_ptr->setNodeKind(NodeKind::ExprK);
				tmp_in_ptr->setKind(ExprKind::CondAndK);
				tmp_in_ptr->appendChild(tmp_ptr);

				tmp_cur_ptr->appendChild(tmp_in_ptr);
				tmp_cur_ptr = tmp_in_ptr;
			} else 
			{
				tmp_cur_ptr->appendChild(tmp_ptr);
				break;
			}
		}

	} else 
	{
		return tmp_ptr;
	}

	return ret;
}

NodePtr analysis::equality_expr()
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG] in equality_expr\n";
#endif
	
	NodePtr ret;
	NodePtr tmp_ptr = rel_expr();

	if ( tmp->getType() == TYPE::EQUOP)
	{
		ret = ::std::make_shared<TreeNode>(tmp->getVal());
		match(tmp->getVal());

		ret->setNodeKind(NodeKind::ExprK);
		ret->setKind(ExprKind::EquK);
		ret->appendChild(tmp_ptr);

		NodePtr tmp_cur_ptr = ret;
		while (1)
		{
			tmp_ptr = rel_expr();

			if ( tmp->getType() == TYPE::EQUOP)
			{
				NodePtr tmp_in_ptr = ::std::make_shared<TreeNode>(tmp->getVal());
				match(tmp->getVal());
				tmp_in_ptr->setNodeKind(NodeKind::ExprK);
				tmp_in_ptr->setKind(ExprKind::EquK);
				tmp_in_ptr->appendChild(tmp_ptr);

				tmp_cur_ptr->appendChild(tmp_in_ptr);
				tmp_cur_ptr = tmp_in_ptr;
			} else 
			{
				tmp_cur_ptr->appendChild(tmp_ptr);
				break;
			}
		}

	} else 
	{
		return tmp_ptr;
	}

	return ret;
}

NodePtr analysis::rel_expr()
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG] in rel_expr\n";
#endif
	
	NodePtr ret;
	NodePtr tmp_ptr = additive_expr();

	if ( tmp->getType() == TYPE::RELOP)
	{
		ret = ::std::make_shared<TreeNode>(tmp->getVal());
		match(tmp->getVal());

		ret->setNodeKind(NodeKind::ExprK);
		ret->setKind(ExprKind::RelK);
		ret->appendChild(tmp_ptr);

		NodePtr tmp_cur_ptr = ret;
		while (1)
		{
			tmp_ptr = additive_expr();

			if ( tmp->getType() == TYPE::RELOP)
			{
				NodePtr tmp_in_ptr = ::std::make_shared<TreeNode>(tmp->getVal());
				match(tmp->getVal());
				tmp_in_ptr->setNodeKind(NodeKind::ExprK);
				tmp_in_ptr->setKind(ExprKind::RelK);
				tmp_in_ptr->appendChild(tmp_ptr);

				tmp_cur_ptr->appendChild(tmp_in_ptr);
				tmp_cur_ptr = tmp_in_ptr;
			} else 
			{
				tmp_cur_ptr->appendChild(tmp_ptr);
				break;
			}
		}

	} else 
	{
		return tmp_ptr;
	}

	return ret;
}

NodePtr analysis::additive_expr()
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG] in additive_expr\n";
#endif
	NodePtr ret;
	NodePtr tmp_ptr = multiplicative_expr();

	if ( tmp->getType() == TYPE::ADDOP)
	{
		ret = ::std::make_shared<TreeNode>(tmp->getVal());
		match(tmp->getVal());

		ret->setNodeKind(NodeKind::ExprK);
		ret->setKind(ExprKind::AdditiveK);
		ret->appendChild(tmp_ptr);

		NodePtr tmp_cur_ptr = ret;
		while (1)
		{
			tmp_ptr = multiplicative_expr();

			if ( tmp->getType() == TYPE::ADDOP)
			{
				NodePtr tmp_in_ptr = ::std::make_shared<TreeNode>(tmp->getVal());
				match(tmp->getVal());
				tmp_in_ptr->setNodeKind(NodeKind::ExprK);
				tmp_in_ptr->setKind(ExprKind::AdditiveK);
				tmp_in_ptr->appendChild(tmp_ptr);

				tmp_cur_ptr->appendChild(tmp_in_ptr);
				tmp_cur_ptr = tmp_in_ptr;
			} else 
			{
				tmp_cur_ptr->appendChild(tmp_ptr);
				break;
			}
		}

	} else 
	{
		return tmp_ptr;
	}

	return ret;
}

NodePtr analysis::multiplicative_expr()
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG] in multiplicative_expr\n";
#endif
	NodePtr ret;
	NodePtr tmp_ptr = unary_expr();

	if ( tmp->getType() == TYPE::MULOP)
	{
		ret = ::std::make_shared<TreeNode>(tmp->getVal());
		match(tmp->getVal());

		ret->setNodeKind(NodeKind::ExprK);
		ret->setKind(ExprKind::MulK);
		ret->appendChild(tmp_ptr);

		NodePtr tmp_cur_ptr = ret;
		while (1)
		{
			tmp_ptr = unary_expr();

			if ( tmp->getType() == TYPE::MULOP)
			{
				NodePtr tmp_in_ptr = ::std::make_shared<TreeNode>(tmp->getVal());
				match(tmp->getVal());
				tmp_in_ptr->setNodeKind(NodeKind::ExprK);
				tmp_in_ptr->setKind(ExprKind::MulK);
				tmp_in_ptr->appendChild(tmp_ptr);

				tmp_cur_ptr->appendChild(tmp_in_ptr);
				tmp_cur_ptr = tmp_in_ptr;
			} else 
			{
				tmp_cur_ptr->appendChild(tmp_ptr);
				break;
			}
		}

	} else 
	{
		return tmp_ptr;
	}

	return ret;
}

NodePtr analysis::unary_expr()
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG] in unary_expr\n";
#endif
	NodePtr ret;
	NodePtr tmp_ptr;
	::std::string tmp_str;

	if ( tmp->getVal() == "+"
			|| tmp->getVal() == "-"
			|| tmp->getVal() == "!")
	{
		ret = ::std::make_shared<TreeNode>(tmp->getVal());
		ret->setNodeKind(NodeKind::ExprK);
		ret->setKind(ExprKind::UnaryK);

		match(tmp->getVal());
	} else 
	{
		ret = primary_expr();
		return ret;
	}

	NodePtr tmp_cur_ptr = ret;
	while(1)
	{
		if ( tmp->getVal() == "+"
			|| tmp->getVal() == "-"
			|| tmp->getVal() == "!")
		{
			NodePtr tmp_in_ptr = ::std::make_shared<TreeNode>(tmp->getVal());
			tmp_in_ptr->setNodeKind(NodeKind::ExprK);
			tmp_in_ptr->setKind(ExprKind::UnaryK);

			match(tmp->getVal());
			tmp_cur_ptr->appendChild(tmp_in_ptr);
			tmp_cur_ptr = tmp_in_ptr;
		} else 
		{
			tmp_ptr = primary_expr();
			tmp_cur_ptr->appendChild(tmp_ptr);
		}
	}

	return ret;
}

NodePtr analysis::primary_expr()
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG] in primary_expr\n";
#endif
	NodePtr ret;

	if ( tmp->getType() == TYPE::NUMINT)
	{
		int tmp_int = numberInteger();
		ret = ::std::make_shared<TreeNode>(tmp_int);
		ret->setNodeKind(NodeKind::ExprK);
		ret->setKind(ExprKind::INTLITERAL);
		ret->setType(TypeKind::IntK);
	} else if ( tmp->getType() == TYPE::NUMFLOAT)
	{
		float tmp_float = numberFloat();
		ret = ::std::make_shared<TreeNode>(tmp_float);
		ret->setNodeKind(NodeKind::ExprK);
		ret->setKind(ExprKind::FLOATLITERAL);
		ret->setType(TypeKind::FloatK);
	} else if ( tmp->getType() == TYPE::NUMBOOLEAN || tmp->getType() == TYPE::STRING)
	{
		ret = ::std::make_shared<TreeNode>(tmp->getVal());

		if ( tmp->getType() == TYPE::NUMBOOLEAN){
			ret->setType(TypeKind::BoolK);
		} else 
		{
			ret->setType(TypeKind::StringK);
		}
		match(tmp->getVal());
		ret->setNodeKind(NodeKind::ExprK);
		ret->setKind(ExprKind::FLOATLITERAL);
	} else if ( tmp->getType() == TYPE::ID)
	{
		::std::string tmp_str = identifier();

		ret = ::std::make_shared<TreeNode>(tmp_str);
		ret->setNodeKind(NodeKind::ExprK);

		if ( tmp->getVal() == "[")
		{
			match("[");
			ret->setKind(ExprKind::ArrayExp);
			NodePtr tmp_ptr = expr();
			match("]");
			ret->appendChild(tmp_ptr);
		} else if ( tmp->getVal() == "(")
		{
			ret->setKind(ExprKind::FuncExp);
			NodePtr tmp_ptr = arg_list();
			ret->appendChild(tmp_ptr);
		} else 
		{
			ret->setKind(ExprKind::IdK);
		}
	} else 
	{
		match("(");
		ret = expr();
		match(")");
	}

	return ret;
}

NodePtr analysis::para_list()
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG] in para_list\n";
#endif
	match("(");
	NodePtr ret = ::std::make_shared<TreeNode>("()");
	NodePtr tmp_ptr = nullptr;
	if ( tmp->getVal() != ")")
	{
		tmp_ptr = proper_para_list();
	}
	ret->appendChild(tmp_ptr);
	match(")");
	return ret;
}

NodePtr analysis::proper_para_list()
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG] in proper_para_list\n";
#endif
	NodePtr ret = para_decl();

	NodePtr tmp_cur_ptr = ret;
	while(tmp->getVal() == ",")
	{
		match(",");
		NodePtr tmp_in_ptr = para_decl();
		tmp_cur_ptr->setSibling(tmp_in_ptr);
		tmp_cur_ptr = tmp_in_ptr;
	}

	return ret;
}

NodePtr analysis::para_decl()
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG] in para_decl\n";
#endif
	NodePtr ret;
	if ( tmp->getType() == TYPE::VOID
			|| tmp->getType() == TYPE::INT
			|| tmp->getType() == TYPE::BOOLEAN
			|| tmp->getType() == TYPE::FLOAT)
	{
		ret = ::std::make_shared<TreeNode>(tmp->getVal());
		ret->setNodeKind(NodeKind::DeclK);
		ret->setKind(DeclKind::ParaK);

		match(tmp->getVal());
	} else 
	{
		::std::cerr << "[ERROR] expect type name near: \""
			<< tmp->getVal() 
			<< "\" in line "
			<< tmp->getLineno()
			<< ::std::endl;
		::std::exit(1);
	}
	NodePtr tmp_ptr = declarator();
	ret->appendChild(tmp_ptr);

	return ret;
}

NodePtr analysis::arg_list()
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG] in arg_list\n";
#endif
	match("(");
	NodePtr ret = ::std::make_shared<TreeNode>("()");
	NodePtr tmp_ptr = nullptr;
	if ( tmp->getVal() != ")")
	{
		tmp_ptr = proper_arg_list();
	}
	ret->appendChild(tmp_ptr);
	match(")");
	return ret;
}

NodePtr analysis::proper_arg_list()
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG] in proper_arg_list\n";
#endif
	NodePtr ret = arg();

	NodePtr tmp_cur_ptr = ret;
	while(tmp->getVal() == ",")
	{
		match(",");
		NodePtr tmp_in_ptr = arg();
		tmp_cur_ptr->setSibling(tmp_in_ptr);
		tmp_cur_ptr = tmp_in_ptr;
	}

	return ret;
}

NodePtr analysis::arg()
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG] in arg\n";
#endif
	NodePtr ret = expr();
	return ret;
}

void analysis::printTree()
{
	if ( initFlag )
	{
		layer_record = 0;
		printTree(_root);
	} 
}

void analysis::printTree(const NodePtr& ptr)
{
	if(ptr != nullptr)
	{
		for( int i = 0; i < layer_record; ++i)
		{
			::std::cout << "  ";
		}
		auto data = ptr->getData();
		::std::cout << ::boost::apply_visitor(get_visitor(),data) << ::std::endl;
		for( auto it: ptr->getChildren())
		{
			layer_record++;
			printTree(it);
			layer_record--;
		}
	} else
	{
		return ;
	}

	if ( ptr->getSibling() != nullptr)
	{
		printTree(ptr->getSibling());
	}
}


}
