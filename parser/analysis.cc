/*
 * Author: Dh
 * Update Date: 2018.5.29
 * Update info: analysis word sequence
 */
#include "../include/analysis.h"
#include "../include/global.h"
#include "../include/scan.h"
#include <stack>
#include <sstream>

namespace dh{

analysis::analysis( scanner tokens)
{
	this->layer_record = 0;
	this->_tokens = tokens;
	this->_root = nullptr;
	this->tmp = ::std::make_shared<token>("", TYPE::NONES, 0);
	initFlag = false;
	this->current_stmt = -1;
	this->tmp_name_counter = 0;
	this->tmp_lable_counter = 0;
	this->genFlag = false;
	this->FLOATLITERALCounter = 0;

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

	symNodePtr tmp1( new symTabElem);
	tmp1->nametype = 'f';
	tmp1->name = "putIntLn";
	tmp1->datatype = TypeKind::VoidK;
	tmp1->args.push_back(TypeKind::IntK);

	this->current_tab = ::std::shared_ptr<symTab>(
			new symTab(""));

	current_tab->insert_elem(tmp1);

	evalType(_root);
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
		//evalType(ret);
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
		//evalType(tmp_in_ptr);

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
	
	NodePtr tmp_ptr = ::std::make_shared<TreeNode>(tmp->getVal(),tmp->getLineno());
	if ( tmp->getVal() == "int")
	{
		tmp_ptr->setType(TypeKind::IntK);
	} else if ( tmp->getVal() == "float")
	{
		tmp_ptr->setType(TypeKind::FloatK); 
	} else if ( tmp->getVal() == "void")
	{
		tmp_ptr->setType(TypeKind::VoidK);
	} else if (tmp->getVal() == "boolean")
	{
		tmp_ptr->setType(TypeKind::BoolK);
	} else 
	{
		::std::cerr << "[ERROR] Unexpected type \""
			<< tmp->getVal() 
			<< "\" in line "
			<< tmp->getLineno()
			<< ::std::endl;
		::std::exit(1);
	}
	match(tmp->getVal());

	int lineno = tmp->getLineno();
	::std::string tmp_str = identifier();
	NodePtr ret = ::std::make_shared<TreeNode>(tmp_str,lineno);
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
	NodePtr ret = ::std::make_shared<TreeNode>(tmp->getVal(), tmp->getLineno());
	if ( tmp->getVal() == "int")
	{
		match("int");
		ret->setType(TypeKind::IntK);
	} else if ( tmp->getVal() == "boolean")
	{
		match("boolean");
		ret->setType(TypeKind::BoolK);
	} else if ( tmp->getVal() == "float")
	{
		match("float");
		ret->setType(TypeKind::FloatK);
	} else if ( tmp->getVal() == "void")
	{
		match("void");
		ret->setType(TypeKind::VoidK);
	} else 
	{
		::std::cerr << "[ERROR] Unexpected type \""
			<< tmp->getVal()
			<< "\" in line "
			<< tmp->getLineno()
			<< ::std::endl;
		::std::exit(1);
	}
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
	int lineno = tmp->getLineno();
	::std::string tmp_str = identifier();
	NodePtr ret = ::std::make_shared<TreeNode>(tmp_str,lineno);
	ret->setNodeKind(NodeKind::DeclaratorK);

	if ( tmp->getVal() == "[")
	{
		lineno = tmp->getLineno();
		match("[");
		NodePtr tmp_ptr;
		if ( tmp->getVal() != "]")
		{
			int tmp_int = numberInteger();
			tmp_ptr = ::std::make_shared<TreeNode>(tmp_int,lineno);
			tmp_ptr->setNodeKind(NodeKind::ExprK);
			tmp_ptr->setKind(ExprKind::INTLITERAL);
			tmp_ptr->setType(TypeKind::IntK);
		} else 
		{
			tmp_ptr = ::std::make_shared<TreeNode>(0,lineno);
			tmp_ptr->setNodeKind(NodeKind::ExprK);
			tmp_ptr->setKind(ExprKind::INTLITERAL);
			tmp_ptr->setType(TypeKind::IntK);
		}
		match("]");
		ret->setKind(DeclaratorKind::ArrayK);
		ret->appendChild(tmp_ptr);
	} else 
	{
		ret->setKind(DeclaratorKind::PrimitiveK);
	}

	ret->setSibling(nullptr);

	return ret;
}

NodePtr analysis::initialiser()
{
#ifdef _DEBUG_
	::std::cout << "[DEBUG] in initialiser\n";
#endif

	NodePtr ret = ::std::make_shared<TreeNode>("=",tmp->getLineno());
	match("=");
	ret->setNodeKind(NodeKind::InitialiserK);
	NodePtr tmp_ptr;

	if ( tmp->getVal() == "{")
	{
		ret->setKind(InitialiserKind::ListK);
		match("{");
		tmp_ptr = expr();

		NodePtr tmp_cur_ptr = tmp_ptr;
		while(tmp->getVal() == ",")
		{
			match(",");
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
	NodePtr ret = ::std::make_shared<TreeNode>("{}",tmp->getLineno());
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
	NodePtr ret = ::std::make_shared<TreeNode>("if",tmp->getLineno());
	match("if");
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
	NodePtr ret= ::std::make_shared<TreeNode>("for",tmp->getLineno());
	match("for");
	NodePtr tmp_ptr;

	ret->setNodeKind(NodeKind::StmtK);
	ret->setKind(StmtKind::ForK);

	match("(");

	if ( tmp->getVal() != ";")
	{
		tmp_ptr = expr();
		ret->appendChild(tmp_ptr);
	} else 
	{
		ret->appendChild(nullptr);
	}
	
	match(";");

	if ( tmp->getVal() != ";")
	{
		tmp_ptr = expr();
		ret->appendChild(tmp_ptr);
	} else 
	{
		ret->appendChild(nullptr);
	}
	match(";");

	if ( tmp->getVal() != ")")
	{
		tmp_ptr = expr();
		ret->appendChild(tmp_ptr);
	} else 
	{
		ret->appendChild(nullptr);
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

	NodePtr ret = ::std::make_shared<TreeNode>("while",tmp->getLineno());
	match("while");
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

	NodePtr ret = ::std::make_shared<TreeNode>("break",tmp->getLineno());
	match("break");
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
	NodePtr ret = ::std::make_shared<TreeNode>("continue",tmp->getLineno());
	match("continue");
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
	NodePtr ret = ::std::make_shared<TreeNode>("return",tmp->getLineno());
	match("return");
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
	NodePtr ret = ::std::make_shared<TreeNode>("Expr",tmp->getLineno());

	ret->setNodeKind(NodeKind::StmtK);
	ret->setKind(StmtKind::ExpK);

	if ( tmp->getType() != TYPE::SEPOP)
	{
		NodePtr tmp_ptr = expr();
		ret->appendChild(tmp_ptr);
	} else 
	{
		ret->appendChild(nullptr);
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
		ret = ::std::make_shared<TreeNode>("=",tmp->getLineno());
		match("=");
		ret->setNodeKind(NodeKind::ExprK);
		ret->setKind(ExprKind::AssignK);
		ret->appendChild(tmp_ptr);

		NodePtr tmp_cur_ptr = ret;
		while (1)
		{
			tmp_ptr = cond_or_expr();

			if ( tmp->getVal() == "=")
			{
				NodePtr tmp_in_ptr = ::std::make_shared<TreeNode>("=",tmp->getLineno());
				match("=");
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
		ret = ::std::make_shared<TreeNode>("||",tmp->getLineno());
		match("||");
		ret->setNodeKind(NodeKind::ExprK);
		ret->setKind(ExprKind::CondOrK);
		ret->appendChild(tmp_ptr);

		NodePtr tmp_cur_ptr = ret;
		while (1)
		{
			tmp_ptr = cond_and_expr();

			if ( tmp->getVal() == "||")
			{
				NodePtr tmp_in_ptr = ::std::make_shared<TreeNode>("||",tmp->getLineno());
				match("||");
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
		ret = ::std::make_shared<TreeNode>("&&",tmp->getLineno());
		match("&&");
		ret->setNodeKind(NodeKind::ExprK);
		ret->setKind(ExprKind::CondAndK);
		ret->appendChild(tmp_ptr);

		NodePtr tmp_cur_ptr = ret;
		while (1)
		{
			tmp_ptr = equality_expr();

			if ( tmp->getVal() == "&&")
			{
				NodePtr tmp_in_ptr = ::std::make_shared<TreeNode>("&&",tmp->getLineno());
				match("&&");
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
		ret = ::std::make_shared<TreeNode>(tmp->getVal(),tmp->getLineno());
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
				NodePtr tmp_in_ptr = ::std::make_shared<TreeNode>(tmp->getVal(),tmp->getLineno());
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
		ret = ::std::make_shared<TreeNode>(tmp->getVal(),tmp->getLineno());
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
				NodePtr tmp_in_ptr = ::std::make_shared<TreeNode>(tmp->getVal(),tmp->getLineno());
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
		ret = ::std::make_shared<TreeNode>(tmp->getVal(),tmp->getLineno());
		match(tmp->getVal());

		ret->setNodeKind(NodeKind::ExprK);
		ret->setKind(ExprKind::AdditiveK);
		ret->appendChild(tmp_ptr);

		NodePtr tmp_cur_ptr = ret;
		while (1)
		{
			tmp_ptr = multiplicative_expr();
			tmp_cur_ptr->appendChild(tmp_ptr);

			if ( tmp->getType() == TYPE::ADDOP)
			{
				NodePtr tmp_in_ptr = ::std::make_shared<TreeNode>(tmp->getVal(),tmp->getLineno());
				match(tmp->getVal());
				tmp_in_ptr->setNodeKind(NodeKind::ExprK);
				tmp_in_ptr->setKind(ExprKind::AdditiveK);
				tmp_in_ptr->appendChild(tmp_cur_ptr);
				tmp_cur_ptr = tmp_in_ptr;
			} else 
			{
				ret = tmp_cur_ptr;
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
		ret = ::std::make_shared<TreeNode>(tmp->getVal(),tmp->getLineno());
		match(tmp->getVal());

		ret->setNodeKind(NodeKind::ExprK);
		ret->setKind(ExprKind::MulK);
		ret->appendChild(tmp_ptr);

		NodePtr tmp_cur_ptr = ret;
		while (1)
		{
			tmp_ptr = unary_expr();
			tmp_cur_ptr->appendChild(tmp_ptr);

			if ( tmp->getType() == TYPE::MULOP)
			{
				NodePtr tmp_in_ptr = ::std::make_shared<TreeNode>(tmp->getVal(),tmp->getLineno());
				match(tmp->getVal());
				tmp_in_ptr->setNodeKind(NodeKind::ExprK);
				tmp_in_ptr->setKind(ExprKind::MulK);
				tmp_in_ptr->appendChild(tmp_cur_ptr);
				tmp_cur_ptr = tmp_in_ptr;
			} else 
			{
				ret = tmp_cur_ptr;
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
		ret = ::std::make_shared<TreeNode>(tmp->getVal(),tmp->getLineno());
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
			NodePtr tmp_in_ptr = ::std::make_shared<TreeNode>(tmp->getVal(),tmp->getLineno());
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

	int lineno = tmp->getLineno();
	if ( tmp->getType() == TYPE::NUMINT)
	{
		int tmp_int = numberInteger();
		ret = ::std::make_shared<TreeNode>(tmp_int,lineno);
		ret->setNodeKind(NodeKind::ExprK);
		ret->setKind(ExprKind::INTLITERAL);
		ret->setType(TypeKind::IntK);
	} else if ( tmp->getType() == TYPE::NUMFLOAT)
	{
		float tmp_float = numberFloat();
		ret = ::std::make_shared<TreeNode>(tmp_float,lineno);
		ret->setNodeKind(NodeKind::ExprK);
		ret->setKind(ExprKind::FLOATLITERAL);
		ret->setType(TypeKind::FloatK);
	} else if ( tmp->getType() == TYPE::NUMBOOLEAN || tmp->getType() == TYPE::STRING)
	{
		ret = ::std::make_shared<TreeNode>(tmp->getVal(),lineno);

		if ( tmp->getType() == TYPE::NUMBOOLEAN){
			ret->setType(TypeKind::BoolK);
			ret->setKind(ExprKind::BOOLLITERAL);
		} else 
		{
			ret->setType(TypeKind::StringK);
			ret->setKind(ExprKind::STRINGLITERAL);
		}
		match(tmp->getVal());
		ret->setNodeKind(NodeKind::ExprK);
	} else if ( tmp->getType() == TYPE::ID)
	{
		::std::string tmp_str = identifier();

		ret = ::std::make_shared<TreeNode>(tmp_str,lineno);
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
	NodePtr ret = ::std::make_shared<TreeNode>("()",tmp->getLineno());
	match("(");
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
		ret = ::std::make_shared<TreeNode>(tmp->getVal(),tmp->getLineno());
		ret->setNodeKind(NodeKind::DeclK);
		ret->setKind(DeclKind::ParaK);

		int tmp_type = tmp->getType();
		switch(tmp_type)
		{
			case TYPE::VOID:
				ret->setType(TypeKind::VoidK);
				break;
			case TYPE::INT:
				ret->setType(TypeKind::IntK);
				break;
			case TYPE::FLOAT:
				ret->setType(TypeKind::FloatK);
				break;
			case TYPE::BOOLEAN:
				ret->setType(TypeKind::BoolK);
				break;
		}
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
	NodePtr ret = ::std::make_shared<TreeNode>("()",tmp->getLineno());
	match("(");
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
		int tmp_type = ptr->getType();
		::std::string tmp_str;
		switch(tmp_type)
		{
			case TypeKind::IntK:
				tmp_str = "int";
				break;
			case TypeKind::BoolK:
				tmp_str = "boolean";
				break;
			case TypeKind::FloatK:
				tmp_str = "float";
				break;
			case TypeKind::VoidK:
				tmp_str = "void";
				break;
			case TypeKind::IntArrayK:
				tmp_str = "intArray";
				break;
			case TypeKind::FloatArrayK:
				tmp_str = "floatArray";
				break;
			case TypeKind::BoolArrayK:
				tmp_str = "boolArray";
				break;
		}
		::std::cout << ::boost::apply_visitor(get_visitor(),data) 
			<< "_" 
			<< tmp_str 
			<<::std::endl;
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

void analysis::evalType(const NodePtr& ptr)
{
	if ( ptr != nullptr)
	{
		switch( ptr->getNodeKind())
		{
			case NodeKind::DeclK:
				switch (ptr->getKind())
				{
					case DeclKind::FuncK:
						{
							ptr->setType(ptr->getChildren().at(0)->getType());

							if ( current_tab == nullptr)
							{
								current_tab = ::std::shared_ptr<symTab>(new symTab
										(""));
							}
							symNodePtr tmp_node(new symTabElem);

							auto data = ptr->getData();
							tmp_node->name = ::boost::apply_visitor(get_visitor(), data);
							tmp_node->nametype = 'f';
							tmp_node->datatype = ptr->getType();

							if ( current_tab->research_elem(tmp_node->name) != nullptr)
							{
								::std::cerr << "[ERROR] duplicate defination of \""
									<< tmp_node->name
									<< "\" in line "
									<< ptr->getLineno()
									<< ::std::endl;

								::std::exit(1);
							}

							// enter sub area
							::std::shared_ptr<symTab> newTab(
									new symTab(tmp_node->name));

							newTab->set_upper_tab(current_tab);
							current_tab->append_down_tab(newTab);
							current_tab = newTab;

							if ( ptr->getChildren()[1]->getChildren().at(0) != nullptr)
							{
								evalType(ptr->getChildren().at(1)->getChildren().at(0));
							}
							current_tab = current_tab->get_upptr_tab();

							// exit sub area and insert function into table
							auto tmp_ptr = ptr->getChildren()[1]->getChildren()[0];
							while ( tmp_ptr != nullptr)
							{
								tmp_node->args.push_back(tmp_ptr->getType());
								tmp_ptr = tmp_ptr->getSibling();
							}
							current_tab->insert_elem(tmp_node);

							// enter again
							current_tab = newTab;
							evalType(ptr->getChildren().at(2));
							current_tab = current_tab->get_upptr_tab();
							// exit sub area
							if ( ptr->getSibling() != nullptr)
							{
								evalType(ptr->getSibling());
							}
							break;
						}
					case DeclKind::VarK:
						this->tmp_dType = ptr->getType();
						evalType(ptr->getChildren().at(0));
						if ( ptr->getSibling() != nullptr)
						{
							evalType(ptr->getSibling());
						}
						break;
					case DeclKind::ParaK:
						{
							this->tmp_dType = ptr->getType();
							evalType(ptr->getChildren().at(0));

							if ( ptr->getSibling() != nullptr)
							{
								evalType(ptr->getSibling());
							}
							break;
						}
				}
				break;
			case NodeKind::DeclaratorK:

				switch(ptr->getKind())
				{
					case DeclaratorKind::ArrayK:
						{
							ptr->setType(tmp_dType+5);
							auto data = ptr->getData();
							auto vname = ::boost::apply_visitor(get_visitor(), data);
							symNodePtr tmp_node(new symTabElem);

							data = ptr->getChildren().at(0)->getData();

							tmp_node->arraylength = ::boost::get<int>(data);
							tmp_node->name = vname;
							tmp_node->nametype = 'a';
							tmp_node->datatype = ptr->getType();

							if ( current_tab->research_elem(vname) != nullptr)
							{
								::std::cerr << "[ERROR] dupilcate defination of \""
									<< vname
									<< "\" in line "
									<< ptr->getLineno()
									<< ::std::endl;

								::std::exit(1);
							}
							current_tab->insert_elem(tmp_node);

							evalType(ptr->getChildren().at(0));
							// if the declarator is float and the initliser can be float or int
							if ( ptr->getChildren().size() == 2)
							{
								evalType(ptr->getChildren().at(1));
								if ( ptr->getChildren().at(1)->getType() != ptr->getType()-5)
								{
									if ( ptr->getType() != TypeKind::FloatK)
									{
										std::cerr << "[ERROR] Incompitable type initialiser with "
											<< "array \""
											<< vname
											<< "\" in line "
											<< ptr->getLineno()
											<< ::std::endl;
										::std::exit(1);
									} else if ( ptr->getChildren().at(1)->getType() != FloatK
											&& ptr->getChildren().at(1)->getType() != IntK)
									{
										std::cerr << "[ERROR] Incompitable type initialiser with "
											<< "array \""
											<< vname
											<< "\" in line "
											<< ptr->getLineno()
											<< ::std::endl;
										::std::exit(1);
									}
								}
							}
							if ( ptr->getChildren().at(0)->getType() != TypeKind::IntK)
							{
								std::cerr << "[ERROR] Only \"int\"-value can be used init "
									<< "array \""
									<< vname
									<< "\" in line "
									<< ptr->getLineno()
									<< ::std::endl;
								::std::exit(1);
							} else if ( ptr->getChildSize() != 1)
							{
								// check if the size of initialiser is match array
								int init_counter = 0;
								auto tmp_ptr = ptr->getChildren().at(1);
								while ( tmp_ptr != nullptr)
								{
									++init_counter;
									tmp_ptr = tmp_ptr->getSibling();
								}

								auto data = ptr->getChildren().at(1)->getData();
								::std::string num = ::boost::apply_visitor(get_visitor(), data);

								::std::stringstream ss;
								int act_counter = 0;
								ss << num;
								ss >> act_counter;

								if ( act_counter == 0)
								{
									act_counter = init_counter;
									ptr->getChildren().at(0)->setData(act_counter);
								} else if ( act_counter < init_counter)
								{
									data = ptr->getData();
									::std::cout << "[ERROR] Incompitable array size with initialiser with \""
										<<  ::boost::apply_visitor(get_visitor(),data)
										<< "\" in line "
										<< ptr->getLineno()
										<< ::std::endl;

									::std::exit(1);
								}
							}
							this->tmp_dType = ptr->getType() - 5;
							break;
						}
					case DeclaratorKind::PrimitiveK:
						{
							ptr->setType(tmp_dType);
							auto data = ptr->getData();
							auto vname = ::boost::apply_visitor(get_visitor(), data);
							symNodePtr tmp_node(new symTabElem);
							tmp_node->name = vname;
							tmp_node->nametype = 'v';
							tmp_node->datatype = ptr->getType();

							if ( current_tab->research_elem(vname) != nullptr)
							{
								::std::cerr << "[ERROR] dupilcate defination of \""
									<< vname
									<< "\" in line "
									<< ptr->getLineno()
									<< ::std::endl;

								::std::exit(1);
							}
							current_tab->insert_elem(tmp_node);

							// if the declarator is float and the initliser can be float or int
							if (ptr->getChildSize() != 0)
							{
								evalType(ptr->getChildren().at(0));

								::std::string tmp_str1;
								::std::string tmp_str2;
								switch(ptr->getType())
								{
									case TypeKind::VoidK:
										tmp_str1 = "void";
										break;
									case TypeKind::IntK:
										tmp_str1 = "int";
										break;
									case TypeKind::FloatK:
										tmp_str1 = "float";
										break;
									case TypeKind::BoolK:
										tmp_str1 = "boolean";
										break;
								}
								switch(ptr->getChildren().at(0)->getType())
								{
									case TypeKind::VoidK:
										tmp_str2 = "void";
										break;
									case TypeKind::IntK:
										tmp_str2 = "int";
										break;
									case TypeKind::FloatK:
										tmp_str2 = "float";
										break;
									case TypeKind::BoolK:
										tmp_str2 = "boolean";
										break;
								}
								if ( ptr->getType() != ptr->getChildren().at(0)->getType())
								{
									if (!((ptr->getChildren().at(0)->getType() == TypeKind::FloatK
												&& ptr->getType() == TypeKind::IntK)
											||( ptr->getChildren().at(0)->getType() == TypeKind::IntK
												&& ptr->getType() == TypeKind::FloatK)))
									{
										std::cerr << "[ERROR] Incompitable type initialiser with "
											<< "variable \""
											<< vname
											<< "\" in line "
											<< ptr->getLineno()
											<< ::std::endl;
										::std::exit(1);
									}
								}
							}
							this->tmp_dType = ptr->getType();
							break;
						}
				}
				if ( ptr->getSibling() != nullptr)
				{
					evalType(ptr->getSibling());
				}
				break;
			case NodeKind::InitialiserK:
				switch(ptr->getKind())
				{
					case InitialiserKind::SingleK:
						evalType(ptr->getChildren().at(0));
						ptr->setType(ptr->getChildren().at(0)->getType());
						break;
					case InitialiserKind::ListK:
						evalType(ptr->getChildren().at(0));
						ptr->setType(ptr->getChildren().at(0)->getType());
						break;
				}
				if ( ptr->getSibling() != nullptr)
				{
					evalType(ptr->getSibling());
				}
				break;
			case NodeKind::StmtK:
				switch(ptr->getKind())
				{
					case StmtKind::ComK:
						{
							::std::shared_ptr<symTab> newTab(
									new symTab( current_tab->get_in_function()));
							newTab->set_upper_tab(current_tab);
							current_tab->append_down_tab(newTab);
							current_tab = newTab;

							for (auto it: ptr->getChildren())
							{
								evalType(it);
							}

							current_tab = current_tab->get_upptr_tab();
							break;
						}
					case StmtKind::ContinueK:
					case StmtKind::BreakK:
						if ( this->current_stmt != StmtKind::WhileK
								&& this->current_stmt != StmtKind::ForK)
						{
							::std::cerr << "[ERROR] expected-unqulified id in line "
								<< ptr->getLineno();
							::std::exit(1);
						}
						break;
					case StmtKind::RetK:
						{
							if ( ptr->getChildSize() == 0)
							{
								ptr->setType(TypeKind::VoidK);
							} else 
							{
								evalType(ptr->getChildren().at(0));
								ptr->setType(ptr->getChildren().at(0)->getType());
							}
							auto tmp_node 
								= current_tab->research_elem_global(
										current_tab->get_in_function());

							::std::string tmp_str1;
							::std::string tmp_str2;
							switch(ptr->getType())
							{
								case TypeKind::VoidK:
									tmp_str1 = "void";
									break;
								case TypeKind::IntK:
									tmp_str1 = "int";
									break;
								case TypeKind::FloatK:
									tmp_str1 = "float";
									break;
								case TypeKind::BoolK:
									tmp_str1 = "boolean";
									break;
							}
							switch(tmp_node->datatype)
							{
								case TypeKind::VoidK:
									tmp_str2 = "void";
									break;
								case TypeKind::IntK:
									tmp_str2 = "int";
									break;
								case TypeKind::FloatK:
									tmp_str2 = "float";
									break;
								case TypeKind::BoolK:
									tmp_str2 = "boolean";
									break;
							}
							if ( ptr->getType() != tmp_node->datatype)
							{
								::std::cerr << "[ERROR] unmatch return type \""
									<< tmp_str1
									<< "\" in function \""
									<< tmp_node->name
									<< "\" expect \""
									<< tmp_str2
									<< "\" in line "
									<< ptr->getLineno()
									<< ::std::endl;

								::std::exit(1);
							}
							break;
						}
					case StmtKind::IfK:
						{
							current_stmt = StmtKind::IfK;

							for(auto it: ptr->getChildren())
							{
								evalType(it);
							}
							if ( ptr->getChildren().at(0)->getType() != BoolK)
							{
								::std::cerr << "[ERROR] Incompitable type expect \"boolean\" in  line "
									<< ptr->getLineno()
									<< ::std::endl;
								::std::exit(1);
							}
							current_tab = current_tab->get_upptr_tab();
							current_stmt = -1;
							break;
						}
					case StmtKind::ForK:
						{
							current_stmt = StmtKind::ForK;
							for( auto it: ptr->getChildren())
							{
								evalType(it);
							}
							current_stmt = -1;

							if ( ptr->getChildren().at(1) != nullptr)
							{
								if ( ptr->getChildren().at(1)->getType() != TypeKind::BoolK)
								{
									::std::cerr << "[ERROR] Need Bool expr in for stmt in line "
										<< ptr->getLineno()
										<< ::std::endl;

									::std::exit(1);
								}
							}
							break;
						}
					case StmtKind::WhileK:
						{
							current_stmt = StmtKind::WhileK;
							for( auto it: ptr->getChildren())
							{
								evalType(it);
							}
							if ( ptr->getChildren().at(0)->getType() != TypeKind::BoolK)
							{
								::std::cerr << "[ERROR] Need Bool expr in for stmt in line "
										<< ptr->getLineno()
										<< ::std::endl;
									::std::exit(1);
							}
							current_stmt = -1;
							break;
						}
					case StmtKind::ExpK:
						evalType(ptr->getChildren().at(0));
						break;
				}
				if ( ptr->getSibling() != nullptr)
				{
					evalType(ptr->getSibling());
				}
				break;
			case NodeKind::ExprK:
				switch( ptr->getKind())
				{
					case ExprKind::BOOLLITERAL:
						ptr->setType(TypeKind::BoolK);
						break;
					case ExprKind::INTLITERAL:
						ptr->setType(TypeKind::IntK);
						break;
					case ExprKind::STRINGLITERAL:
						ptr->setType(TypeKind::StringK);
						break;
					case ExprKind::FLOATLITERAL:
						ptr->setType(TypeKind::FloatK);
						break;
					case ExprKind::RelK:
					case ExprKind::EquK:
					case ExprKind::CondOrK:
					case ExprKind::CondAndK:
						{
							evalType(ptr->getChildren().at(0));
							evalType(ptr->getChildren().at(1));

							::std::string tmp_type1;
							::std::string tmp_type0;
							switch(ptr->getChildren().at(0)->getType())
							{
								case TypeKind::BoolK:
									tmp_type0 = "boolean";
									break;
								case TypeKind::StringK:
									tmp_type0 = "string";
									break;
								case TypeKind::VoidK:
									tmp_type0 = "void";
									break;
								case TypeKind::FloatK:
									tmp_type0 = "float";
									break;
								case TypeKind::IntK:
									tmp_type0 = "int";
									break;
							}

							switch(ptr->getChildren().at(1)->getType())
							{
								case TypeKind::BoolK:
									tmp_type1 = "boolean";
									break;
								case TypeKind::StringK:
									tmp_type1 = "string";
									break;
								case TypeKind::VoidK:
									tmp_type1 = "void";
									break;
								case TypeKind::IntK:
									tmp_type1 = "int";
									break;
								case TypeKind::FloatK:
									tmp_type1 = "float";
									break;
							}

							if ( ptr->getChildren().at(0)->getType()
									== ptr->getChildren().at(1)->getType())
							{
								ptr->setType(TypeKind::BoolK);
							} else if ( ptr->getChildren().at(0)->getType() == FloatK
									&& ptr->getChildren().at(1)->getType() == IntK)
							{
								ptr->setType(TypeKind::BoolK);
							} else if ( ptr->getChildren().at(0)->getType() == IntK
									&& ptr->getChildren().at(0)->getType() == FloatK)
							{
								ptr->setType(TypeKind::BoolK);
							} else 
							{
								auto data = ptr->getData();
								::std::cerr << "[ERROR] can't convert \""
									<< tmp_type1 
									<< "\" to \""
									<< tmp_type0
									<< "\" with \""
									<< ::boost::apply_visitor(get_visitor(), data)
									<< "\" in line "
									<< ptr->getLineno()
									<< ::std::endl;
								::std::exit(1);
							}
							break;
						}
					case ExprKind::MulK:
					case ExprKind::AdditiveK:
					case ExprKind::AssignK:
						{
							evalType(ptr->getChildren().at(0));
							evalType(ptr->getChildren().at(1));

							if ( ptr->getKind() == ExprKind::MulK 
									|| ptr->getKind() == ExprKind::AdditiveK)
							{
								if ( (ptr->getChildren().at(0)->getType() != IntK
											&& ptr->getChildren().at(0)->getType() != FloatK)
										|| (ptr->getChildren().at(1)->getType() != FloatK
											&& ptr->getChildren().at(1)->getType() != IntK))
								{
									::std::cout << "[ERROR] Type not support operator in line "
										<< ptr->getLineno()
										<< ::std::endl;
									::std::exit(1);
								}
							}

							::std::string tmp_type1;
							::std::string tmp_type0;
							switch(ptr->getChildren().at(0)->getType())
							{
								case TypeKind::BoolK:
									tmp_type0 = "boolean";
									break;
								case TypeKind::StringK:
									tmp_type0 = "string";
									break;
								case TypeKind::VoidK:
									tmp_type0 = "void";
									break;
								case TypeKind::IntK:
									tmp_type0 = "int";
									break;
								case TypeKind::FloatK:
									tmp_type0 = "float";
									break;
							}

							switch(ptr->getChildren().at(1)->getType())
							{
								case TypeKind::BoolK:
									tmp_type1 = "boolean";
									break;
								case TypeKind::StringK:
									tmp_type1 = "string";
									break;
								case TypeKind::VoidK:
									tmp_type1 = "void";
									break;
								case TypeKind::IntK:
									tmp_type1 = "int";
									break;
								case TypeKind::FloatK:
									tmp_type1 = "float";
									break;
							}

							if (ptr->getKind() == AssignK)
							{
								if ( ptr->getChildren().at(0)->getType()
										== ptr->getChildren().at(1)->getType())
								{
									ptr->setType(ptr->getChildren().at(0)->getType());
								} else if ( ptr->getChildren().at(0)->getType() == FloatK
										&& ptr->getChildren().at(1)->getType() == IntK)
								{
									ptr->setType(FloatK);
								} else if ( ptr->getChildren().at(0)->getType() == IntK
										&& ptr->getChildren().at(1)->getType() == FloatK)
								{
									ptr->setType(IntK);
								} else 
								{
									auto data = ptr->getData();
									::std::cerr << "[ERROR] can't convert \""
										<< tmp_type1 
										<< "\" to \""
										<< tmp_type0
										<< "\" with \""
										<< ::boost::apply_visitor(get_visitor(), data)
										<< "\" in line "
										<< ptr->getLineno()
										<< ::std::endl;
									::std::exit(1);
								}
							} else 
							{
								if ( ptr->getChildren().at(0)->getType()
										== ptr->getChildren().at(1)->getType())
								{
									ptr->setType(ptr->getChildren().at(0)->getType());
								} else if ( ptr->getChildren().at(0)->getType() == FloatK
										&& ptr->getChildren().at(1)->getType() == IntK)
								{
									ptr->setType(FloatK);
								} else if ( ptr->getChildren().at(0)->getType() == IntK
										&& ptr->getChildren().at(1)->getType() == FloatK)
								{
									ptr->setType(FloatK);
								} else 
								{
									auto data = ptr->getData();
									::std::cerr << "[ERROR] can't convert \""
										<< tmp_type1 
										<< "\" to \""
										<< tmp_type0
										<< "\" with \""
										<< ::boost::apply_visitor(get_visitor(), data)
										<< "\" in line "
										<< ptr->getLineno()
										<< ::std::endl;
									::std::exit(1);
								}
							}
							break;
						}
					case ExprKind::UnaryK:
						{
							evalType(ptr->getChildren().at(0));

							::std::string tmp_type0;
							switch(ptr->getChildren().at(0)->getType())
							{
								case TypeKind::BoolK:
									tmp_type0 = "boolean";
									break;
								case TypeKind::StringK:
									tmp_type0 = "string";
									break;
								case TypeKind::VoidK:
									tmp_type0 = "void";
									break;
								case TypeKind::IntK:
									tmp_type0 = "int";
									break;
								case TypeKind::FloatK:
									tmp_type0 = "float";
									break;
							}
							auto data = ptr->getData();
							::std::string tmp_str = 
								::boost::apply_visitor(get_visitor(), data);
							if ( tmp_str == "+" || tmp_str == "-")
							{
								if ( ptr->getChildren().at(0)->getType() != IntK
										&& ptr->getChildren().at(0)->getType() != FloatK)
								{
									::std::cerr << "[ERROR] Incompitable type \""
										<< tmp_type0 
										<< "\" with unary operator \"+\" or \"-\" in line"
										<< ptr->getLineno()
										<< ::std::endl;
									::std::exit(1);
								}
								ptr->setType(ptr->getChildren().at(0)->getType());
							} else 
							{
								if ( ptr->getChildren().at(0)->getType() == VoidK
										|| ptr->getChildren().at(0)->getType() == StringK)
								{
									::std::cerr << "[ERROR] Incompitable type \""
										<< tmp_type0 
										<< "\" with unary operator \"!\" in line"
										<< ptr->getLineno()
										<< ::std::endl;
									::std::exit(1);
								}
								ptr->setType(TypeKind::BoolK);
							}
							break;
						}
					case ExprKind::BraketExp:
						{
							evalType(ptr->getChildren().at(0));
							ptr->setType(ptr->getChildren().at(0)->getType());
							break;
						}
					case ExprKind::IdK:
						{
							auto data = ptr->getData();
							::std::string name = ::boost::apply_visitor(get_visitor(), data);
							auto tmp_node = this->current_tab->research_elem_global(name);
							if ( tmp_node == nullptr)
							{
								::std::cerr << "[ERROR] Unknown token \""
									<< name
									<< "\" in line "
									<< ptr->getLineno()
									<< ::std::endl;
								::std::exit(1);
							} else 
							{
								::std::string tmp_str;
								switch(tmp_node->nametype)
								{
									case 'i':
										tmp_str = "identifier";
										break;
									case 'a':
										tmp_str = "array";
										break;
									case 'f':
										tmp_str = "function";
										break;
								}
								if ( tmp_node->nametype != 'v'){
									::std::cerr << "[ERROR] \""
										<< name
										<< "\" in line "
										<< ptr->getLineno()
										<< " is not a valid identifier but a "
										<< tmp_str
										<< ::std::endl;
									::std::exit(1);
								}
								ptr->setType(tmp_node->datatype);
							}
							break;
						}
					case ExprKind::FuncExp:
						{
							auto data = ptr->getData();
							::std::string name = ::boost::apply_visitor(get_visitor(), data);
							auto tmp_node = this->current_tab->research_elem_global(name);
							if ( tmp_node == nullptr)
							{
								::std::cerr << "[ERROR] Unknown token \""
									<< name
									<< "\" in line "
									<< ptr->getLineno()
									<< ::std::endl;
								::std::exit(1);
							} else 
							{
								::std::string tmp_str;
								switch(tmp_node->nametype)
								{
									case 'i':
										tmp_str = "identifier";
										break;
									case 'a':
										tmp_str = "array";
										break;
									case 'f':
										tmp_str = "function";
										break;
								}
								if ( tmp_node->nametype != 'f'){
									::std::cerr << "[ERROR] \""
										<< name
										<< "\" in line "
										<< ptr->getLineno()
										<< "is not a valid function but a "
										<< tmp_str
										<< ::std::endl;
									::std::exit(1);
								}
								ptr->setType(tmp_node->datatype);
								evalType(ptr->getChildren().at(0)->getChildren().at(0));

								size_t counter = 0;
								auto tmp = ptr->getChildren().at(0)->getChildren().at(0);
								while( tmp != nullptr)
								{
									if ( counter >= tmp_node->args.size())
									{
										::std::cerr << "[ERROR] can't find match size args function \""
											<< tmp_node->name
											<< "\" in line "
											<< ::std::endl;

										::std::exit(1);
									}
									if ( tmp->getType() != tmp_node->args.at(counter++))
									{
										::std::cerr << "[ERROR] can't find match types args function \""
											<< tmp_node->name
											<< "\" in line "
											<< ::std::endl;

										::std::exit(1);
									}
									tmp = tmp->getSibling();
								}
							}
							break;
						}
					case ExprKind::ArrayExp:
						{
							auto data = ptr->getData();
							::std::string name = ::boost::apply_visitor(get_visitor(), data);
							auto tmp_node = this->current_tab->research_elem_global(name);
							if ( tmp_node == nullptr)
							{
								::std::cerr << "[ERROR] Unknown token \""
									<< name
									<< "\" in line "
									<< ptr->getLineno()
									<< ::std::endl;
								::std::exit(1);
							} else 
							{
								::std::string tmp_str;
								switch(tmp_node->nametype)
								{
									case 'i':
										tmp_str = "identifier";
										break;
									case 'a':
										tmp_str = "array name";
										break;
									case 'f':
										tmp_str = "function";
										break;
								}
								if ( tmp_node->nametype != 'a')
								{
									::std::cerr << "[ERROR] \""
										<< name
										<< "\" in line "
										<< ptr->getLineno()
										<< "is not a valid array but a "
										<< tmp_str
										<< ::std::endl;
									::std::exit(1);
								}
								ptr->setType(tmp_node->datatype-5);
								evalType(ptr->getChildren().at(0));

								if ( ptr->getChildren().at(0)->getType() != TypeKind::IntK)
								{
									::std::cerr << "[ERROR] Subscript can only be \"int\" with \""
										<< tmp_node->name
										<< "\" in line "
										<< ptr->getLineno()
										<< ::std::endl;
									::std::exit(1);
								}
							}
							break;
						}
				}
				if ( ptr->getSibling() != nullptr)
				{
					evalType(ptr->getSibling());
				}
				break;
		}
	}
}

::std::string analysis::newtmpVal()
{
	::std::stringstream ss;
	ss << "@" << tmp_name_counter++;
	return ss.str();
}

::std::string analysis::newtmpLab()
{
	::std::stringstream ss;
	ss << ".L" << tmp_lable_counter++;
	return ss.str();
}

void analysis::genMidCode( const NodePtr& ptr, const ::std::string& label1, const ::std::string & label2)
{
	if ( ptr != nullptr)
	{
		int nodeKind = ptr->getNodeKind();
		int kind = ptr->getKind();

		switch(nodeKind)
		{
			case NodeKind::InitialiserK:
				{
					genMidCode(ptr->getChildren().at(0), label1, label2);
					break;
				}
			case NodeKind::DeclK:
				if ( kind == DeclKind::FuncK)
				{
					auto data = ptr->getData();
					auto tmp_str = ::boost::apply_visitor(get_visitor(), data);
					auto tmp = trival("entry", trivalitem(tmp_str,ptr->getType()));
					midcodes.push_back(tmp);

					genMidCode(ptr->getChildren().at(2), label1, label2);

					midcodes.push_back(trival("end entry"));
					break;
				} else if ( kind == DeclKind::VarK)
				{
					genMidCode(ptr->getChildren().at(0), label1, label2);
					break;
				}
			case NodeKind::StmtK:
				switch( kind)
				{
					case StmtKind::IfK:
						{
							genMidCode(ptr->getChildren().at(0), label1, label2);

							auto lab1 = newtmpLab();
							auto tmp = 
								trival(
									trivalitem(ptr->getChildren().at(0)->getStrVal(), 
										TypeKind::BoolK),
									"cmp",
									trivalitem("false",
										TypeKind::BoolK)
									);
							midcodes.push_back(tmp);

							tmp = trival( "je",
									trivalitem(lab1, -1));
							midcodes.push_back(tmp);

							genMidCode(ptr->getChildren().at(1), label1, label2);

							::std::string lab2;
							if ( ptr->getChildSize() > 2)
							{
								lab2 = newtmpLab();
								tmp = trival( "jmp",
										trivalitem(lab2, -1));
								midcodes.push_back(tmp);
							}

							tmp = trival("lable",
									trivalitem(lab1, -1));
							midcodes.push_back(tmp);

							if ( ptr->getChildSize() > 2)
							{
								genMidCode( ptr->getChildren().at(2), label1, label2);
								tmp = trival("lable",
										trivalitem(lab2, -1));
								midcodes.push_back(tmp);
							}
							break;
						}
					case StmtKind::WhileK:
						{
							auto lab1 = newtmpLab();
							
							auto tmp = trival("lable", trivalitem(lab1, -1));
							midcodes.push_back(tmp);

							genMidCode(ptr->getChildren().at(0), label1, label2);

							auto lab2 = newtmpLab();
							tmp = trival(
									trivalitem(ptr->getChildren().at(0)->getStrVal(), 
										ptr->getChildren().at(0)->getType()),
									"cmp",
									trivalitem("false",
										TypeKind::BoolK));
							midcodes.push_back(tmp);

							tmp = trival("je", trivalitem(lab2, -1));
							midcodes.push_back(tmp);

							genMidCode(ptr->getChildren().at(1), lab1, lab2);

							tmp = trival("jmp", trivalitem(lab1, -1));
							midcodes.push_back(tmp);

							tmp = trival("label", trivalitem(lab2, -1));
							midcodes.push_back(tmp);
							break;
						}
					case StmtKind::BreakK:
						{
							auto tmp = trival("jmp", trivalitem(label2, -1));
							midcodes.push_back(tmp);
							break;
						}
					case StmtKind::ContinueK:
						{
							auto tmp = trival("jmp", trivalitem(label1, -1));
							midcodes.push_back(tmp);
							break;
						}
					case StmtKind::RetK:
						{
							if ( ptr->getType() != TypeKind::VoidK)
							{
								genMidCode(ptr->getChildren().at(0), label1, label2);

								auto tmp = trival("return", trivalitem(
											trivalitem(ptr->getChildren().at(0)->getStrVal(), 
												ptr->getType())));
								midcodes.push_back(tmp);
							}
							midcodes.push_back(trival("end func"));
							break;
						}
					case StmtKind::ForK:
						{
							auto lab1 = newtmpLab();
							auto lab2 = newtmpLab();

							genMidCode(ptr->getChildren().at(0), label1, label2);
							auto tmp = trival("label", 
									trivalitem(lab1, -1));
							midcodes.push_back(tmp);

							genMidCode(ptr->getChildren().at(1), label1, label2);

							if ( ptr->getChildren().at(1) != nullptr)
							{
								tmp = trival(
										trivalitem(ptr->getChildren().at(1)->getStrVal(),
											TypeKind::BoolK),
										"cmp",
										trivalitem("false",
											TypeKind::BoolK));
								midcodes.push_back(tmp);

								tmp = trival("je", 
										trivalitem(lab2, -1));
								midcodes.push_back(tmp);
							}
							genMidCode(ptr->getChildren().at(3), lab1, lab2);
							genMidCode(ptr->getChildren().at(2), label1, label2);

							tmp = trival("jmp",
									trivalitem(lab1, -1));
							midcodes.push_back(tmp);

							tmp = trival("label",
									trivalitem(lab2, -1));
							midcodes.push_back(tmp);

							break;
						}
					case StmtKind::ComK:
						{
							auto tmp = trival("compound");
							midcodes.push_back(tmp);
							for ( auto it : ptr->getChildren())
							{
								genMidCode( it, label1, label2);
							}
							tmp = trival("end compound");
							midcodes.push_back(tmp);
							break;
						}
					case StmtKind::ExpK:
						{
							genMidCode(ptr->getChildren().at(0), label1, label2);
							break;
						}
				}
				break;
			case NodeKind::DeclaratorK:
				{
					if ( kind == DeclaratorKind::PrimitiveK)
					{
						if ( ptr->getChildSize() != 0)
						{
							genMidCode(ptr->getChildren().at(0), label1, label2);

							auto data = ptr->getData();
							auto tmp_str = ::boost::apply_visitor(get_visitor(), data);
							auto tmp = trival(
									trivalitem( tmp_str, 
										ptr->getType()),
									"=",
									trivalitem( ptr->getChildren().at(0)->getChildren().at(0)->getStrVal(), 
										ptr->getChildren().at(0)->getType()));
							midcodes.push_back(tmp);
						}
						break;
					} else if ( kind == DeclaratorKind::ArrayK)
					{
						if ( ptr->getChildSize() == 2)
						{
							genMidCode( ptr->getChildren().at(0), label1, label2);

							int counter = 0;
							auto tmp_ptr = ptr->getChildren().at(1)->getChildren().at(0);

							while( tmp_ptr != nullptr)
							{
								::std::stringstream ss;
								ss << counter++;
								genMidCode( tmp_ptr, label1, label2);

								auto data = ptr->getData();
								auto node_name = ::boost::apply_visitor(get_visitor(), data);
								auto tmp_str = "elem_size("+node_name+")";

								auto tmpname1 = newtmpVal();
								auto tmp = trival(
										trivalitem(tmpname1, 
											TypeKind::IntK),
										"*",
										trivalitem(ss.str(),
											TypeKind::IntK),
										trivalitem(tmp_str,
											TypeKind::IntK));
								midcodes.push_back(tmp);

								auto tmpname2 = newtmpVal();
								tmp = trival(
										trivalitem( tmpname2, 
											TypeKind::IntK),
										"+",
										trivalitem("&"+node_name,
											TypeKind::IntK),
										trivalitem(tmpname1,
											TypeKind::IntK));
								midcodes.push_back(tmp);

								tmp = trival(
										trivalitem( "*"+tmpname2,
											ptr->getType()-5),
										"=",
										trivalitem( tmp_ptr->getStrVal(),
											tmp_ptr->getKind()));
								midcodes.push_back(tmp);

								tmp_ptr = tmp_ptr->getSibling();
							}
							break;
						}
					}
				}
				break;
			case NodeKind::ExprK:
				{
					switch(kind)
					{
						case ExprKind::CondOrK:
						case ExprKind::CondAndK:
						case ExprKind::EquK:
						case ExprKind::MulK:
						case ExprKind::RelK:
						case ExprKind::AdditiveK:
							{
								genMidCode(ptr->getChildren().at(0), label1, label2);
								genMidCode(ptr->getChildren().at(1), label1, label2);

								auto data = ptr->getData();
								auto node_name = ::boost::apply_visitor(get_visitor(), data);
								auto tmpname = newtmpVal();

								ptr->setStrVal(tmpname);
								auto tmp = trival(
										trivalitem(tmpname,
											ptr->getType()),
										node_name,
										trivalitem(ptr->getChildren().at(0)->getStrVal(),
											ptr->getChildren().at(0)->getType()),
										trivalitem(ptr->getChildren().at(1)->getStrVal(),
											ptr->getChildren().at(1)->getType()));
								midcodes.push_back(tmp);
								break;
							}
						case ExprKind::AssignK:
							{
								genMidCode(ptr->getChildren().at(0), label1, label2);
								genMidCode(ptr->getChildren().at(1), label1, label2);

								auto data = ptr->getData();
								auto node_name = ::boost::apply_visitor(get_visitor(), data);
								auto tmpname = newtmpVal();

								data = ptr->getChildren().at(0)->getData();

								ptr->setStrVal(tmpname);
								auto tmp = trival(
										trivalitem(ptr->getChildren().at(0)->getStrVal(),
											ptr->getChildren().at(0)->getType()),
										node_name,
										trivalitem(ptr->getChildren().at(1)->getStrVal(),
											ptr->getChildren().at(1)->getType()));
								midcodes.push_back(tmp);
								break;
							}
						case ExprKind::UnaryK:
							{
								genMidCode(ptr->getChildren().at(0), label1, label2);

								auto data = ptr->getData();
								auto node_name = ::boost::apply_visitor(get_visitor(), data);
								auto tmpname = newtmpVal();

								ptr->setStrVal(tmpname);
								if ( node_name != "!"){
									auto tmp = trival(
											trivalitem(tmpname,
												ptr->getType()),
											node_name,
											trivalitem(ptr->getChildren().at(0)->getStrVal(),
												ptr->getChildren().at(0)->getType()),
											trivalitem("0",
												ptr->getChildren().at(0)->getType()));
									midcodes.push_back(tmp);
								} else 
								{
									auto tmp = trival(
											trivalitem(tmpname,
												ptr->getType()),
											"=not",
											trivalitem(ptr->getChildren().at(0)->getStrVal(),
												ptr->getChildren().at(0)->getType()));
									midcodes.push_back(tmp);
								}

								break;
							}
						case ExprKind::ArrayExp:
							{
								genMidCode(ptr->getChildren().at(0), label1, label2);

								auto data = ptr->getData();
								auto name = ::boost::apply_visitor(get_visitor(), data);
								auto tmp_str = "elem(" + name + ")";

								auto tmpname1 = newtmpVal();
								auto tmp = trival(
										trivalitem( tmpname1, 
											TypeKind::IntK),
										"*",
										trivalitem( ptr->getChildren().at(0)->getStrVal(),
											TypeKind::IntK),
										trivalitem( tmp_str,
											TypeKind::IntK));
								midcodes.push_back(tmp);

								auto tmpname2 = newtmpVal();
								tmp = trival( 
										trivalitem( tmpname2,
											TypeKind::IntK),
										"+",
										trivalitem( "&" + name,
											TypeKind::IntK),
										trivalitem( tmpname1,
											TypeKind::IntK));
								midcodes.push_back(tmp);

								ptr->setStrVal("*" + tmpname2);
								break;
							}
						case ExprKind::FuncExp:
							{
								auto tmp = trival("begain_args");
								midcodes.push_back(tmp);
								auto tmp_ptr = ptr->getChildren().at(0)->getChildren().at(0);

								while ( tmp_ptr != nullptr)
								{
									genMidCode(tmp_ptr, label1, label2);

									tmp = trival("arg",
											trivalitem( tmp_ptr->getStrVal(),
												tmp_ptr->getType()));
									midcodes.push_back(tmp);

									tmp_ptr = tmp_ptr->getSibling();
								}

								if ( ptr->getType() == TypeKind::VoidK)
								{
									auto data = ptr->getData();
									auto tmp_str = ::boost::apply_visitor(get_visitor(),data);
									tmp = trival( 
											"call",
											trivalitem(tmp_str,
												ptr->getType()));
								} else 
								{
									auto tmpname = newtmpVal();
									auto data = ptr->getData();
									auto tmp_str = ::boost::apply_visitor(get_visitor(),data);
									tmp = trival( 
											trivalitem(tmpname,
												ptr->getType()),
											"=call",
											trivalitem(tmp_str,
												ptr->getType()));
									ptr->setStrVal(tmpname);
								}
								midcodes.push_back(tmp);
								break;
							}
						case ExprKind::FLOATLITERAL:
						case ExprKind::INTLITERAL:
							{
								auto data = ptr->getData();
								symNodePtr tmpnode (new symTabElem);
								tmpnode->name = ::boost::apply_visitor(get_visitor(), data);
								tmpnode->nametype = 0;

								auto ptr = current_tab->research_elem_global(tmpnode->name);
								if ( ptr == nullptr)
								{
									tmpnode->floatcounter = FLOATLITERALCounter++;
									current_tab->insert_elem(tmpnode);
								}
							}
						case ExprKind::BOOLLITERAL:
						case ExprKind::IdK:
							{
								auto data = ptr->getData();
								auto node_name = ::boost::apply_visitor(get_visitor(), data);

								ptr->setStrVal( node_name);
								break;
							}
					}
				}
				break;
		}

		if ( ptr->getSibling() != nullptr)
		{
			genMidCode(ptr->getSibling(), label1, label2);
		}
	}
}

void analysis::genMidCode()
{
	if ( !initFlag)
	{
		::std::cerr << "[Fatal] SynTree not ready" << ::std::endl;
		::std::exit(1);
	}

	while ( current_tab->get_upptr_tab() != nullptr)
	{
		current_tab = current_tab->get_upptr_tab();
	}
	genMidCode( _root, "", "");
	genFlag = true;
}

void analysis::printMidCode()
{
	for ( auto it: midcodes)
	{
		switch(it.getSize())
		{
			case 0:
				::std::cout << it.getOperand() << ::std::endl;
				break;
			case 1:
				::std::cout << it.getOperand()
					<< " "
					<< it.getVal1().name
					<< "_"
					<< it.getVal1().type
					<< ::std::endl;
				break;
			case 2:
				::std::cout << it.getRes().name
					<< "_"
					<< it.getRes().type
					<< " "
					<< it.getOperand()
					<< " "
					<< it.getVal1().name
					<< "_"
					<< it.getVal1().type
					<< ::std::endl;
				break;
			case 3:
				::std::cout << it.getRes().name
					<< "_"
					<< it.getRes().type
					<< " = "
					<< it.getVal1().name
					<< "_"
					<< it.getVal1().type
					<< " "
					<< it.getOperand()
					<< " "
					<< it.getVal2().name
					<< "_"
					<< it.getVal2().type
					<< ::std::endl;
				break;
		}
	}
}

void analysis::genCode()
{
	::std::vector< ::std::string > literal;
	::std::stack< int > counters;
	auto now_tab = current_tab;
	int counter = 0;
	int funccounter = 0;

	::std::cout << "\t.file\t\"target.c\"\n";
	::std::cout << "\t.intel_syntax noprefix\n";
	::std::cout << "\t.text\n";

	for (auto it : midcodes)
	{
		if ( it.getOperand() == "entry")
		{
			::std::cout << "\t.globl\t" << it.getVal1().name << ::std::endl;
			::std::cout << "\t.type\t" << it.getVal1().name << ", @function" << ::std::endl;
			auto ptrs = 
				current_tab->get_down_tabs();
			for( auto ptr : ptrs)
			{
				if ( ptr->get_in_function() == it.getVal1().name)
				{
					now_tab = ptr;
					break;
				}
			}
			::std::cout << it.getVal1().name << ":\n";
			::std::cout << ".LFB" << funccounter << ::std::endl;
			::std::cout << "\t.cfi_startproc\n";
			::std::cout << "\tpush\trbp\n";
			::std::cout << "\tcfi_def_cfa_offset 16\n";
			::std::cout << "\tmov \trbp, rsp\n";
			::std::cout << "\t.cfi_def_cfa_register 6\n";
		} else if (it.getOperand() == "end func") {

			::std::cout << "11\n";
			::std::cout << "\tpop\trbp\n";
			::std::cout << "\tret\n";
		} else if ( it.getOperand() == "end entry")
		{
			::std::cout << "2\n";
			counter = 0;
			while ( !counters.empty())
			{
				counters.pop();
			}
			if ( now_tab->get_in_function() == "main")
			{
				::std::cout << "\tmov\teax, 0\n";
				::std::cout << "\tleave\n";
			} else 
			{
				::std::cout << "\tpop\trbp\n";
			}
			::std::cout << "\t.cif_def_cfa 7, 8\n";
			::std::cout << "\tret\n";
			::std::cout << "\t.cfi_endproc\n";
			::std::cout << ".LFE" << funccounter << ::std::endl;
			::std::cout << "\t.size\t" 
				<< now_tab->get_in_function() << ", .-"
				<< now_tab->get_in_function() << ::std::endl;
			++funccounter;
			now_tab = current_tab->get_upptr_tab();
		} else if ( it.getOperand() == "compound") 
		{
			::std::cout << "3\n";
			counters.push(counter);
			now_tab = now_tab->get_down_tabs().at(counter);
			counter = 0;
		} else if ( it.getOperand() == "end compound") 
		{
			::std::cout << "4\n";
			counter = counters.top();
			counters.pop();
			now_tab = now_tab->get_upptr_tab();
			++counter;
		} else if ( it.getOperand() == "label")
		{
			::std::cout << "5\n";
			::std::cout << it.getVal1().name << ":\n";
		} else if ( it.getOperand() == "je" || it.getOperand() == "jmp")
		{
			::std::cout << "6\n";
			::std::cout << it.getOperand()
				<< "\t" << it.getVal1().name 
				<< ::std::endl;
		} else if ( it.getOperand() == "+") 
		{
			::std::cout << "10\n";
			auto ptr0 = 
				now_tab->research_elem_global(it.getVal1().name);
			auto ptr1 =
				now_tab->research_elem_global(it.getVal2().name);
			if ( ptr0 == nullptr && ptr1 != nullptr)
			{
				if ( ptr1->nametype == 0) 
				{
					if ( it.getVal1().type == TypeKind::IntK)
					{
						::std::cout << "\tadd\teax, " << ptr1->name << ::std::endl;
						if ( it.getRes().type == FloatK)
						{
							::std::cout << "\tpxor\txmm0, xmm0\n";
							::std::cout << "\tcvtsi2ss\txmm0, eax\n";
						}
					} else if ( it.getVal1().type == TypeKind::FloatK)
					{
						bool flag = false;
						for ( auto x : literal)
						{
							if ( x == ptr1->name)
							{
								flag = true;
								break;
							}
						}
						if ( !flag)
						{
							literal.push_back(ptr1->name);
						}
						::std::cout << "\tmovaps\txmm1, xmm0\n";
						::std::cout << "\tmovss\txmm0, .L"
							<< ptr1->floatcounter
							<< "[rip]\n";
						::std::cout << "\taddss\txmm0, xmm1\n";
						if ( it.getRes().type == IntK)
						{
							::std::cout << "\tcvttss2si\teax, xmm0";
						}
					}
				} else if ( it.getVal1().type == TypeKind::IntK)
				{
					if ( it.getVal2().type == TypeKind::IntK)
					{
						::std::cout << "\tadd\teax, [rbp-"
							<< ptr1->offset
							<< "]\n";
						if ( it.getRes().type == TypeKind::FloatK)
						{
							::std::cout << "\tpxor\txmm0, xmm0\n";
							::std::cout << "\tcvtsi2ss\txmm0, eax\n";
						}
					} else if ( it.getVal2().type == TypeKind::FloatK)
					{
						if ( it.getRes().type == TypeKind::IntK)
						{
							::std::cout << "\tpxor\txmm0, xmm0\n";
							::std::cout << "\tcvtsi2ss\txmm0, eax\n";
							::std::cout << "\taddss\txmm0, DWORD PTR [rbp-"
								<< ptr1->offset
								<< "]\n";
							::std::cout << "\tcvttss2si\teax, xmm0\n";
						} else if ( it.getRes().type == TypeKind::FloatK)
						{
							::std::cout << "\tpxor\txmm0, xmm0\n";
							::std::cout << "\tcvtsi2ss\txmm0, eax\n";
							::std::cout << "\tmovss\txmm1, DWORD PTR [rbp-"
								<< ptr1->offset
								<< "]\n";
							::std::cout << "\taddss\txmm0, xmm1\n";
						}
					}
				} else if ( it.getVal1().type == TypeKind::FloatK)
				{
					if ( it.getVal2().type == TypeKind::FloatK)
					{
						::std::cout << "\taddss xmm0, DWORD PTR [rbp-"
							<< ptr1->offset
							<< "]\n";
						if ( it.getRes().type == IntK)
						{
							::std::cout << "\tcvttss2si\teax, xmm0\n";
						}
					} else if ( it.getVal2().type == TypeKind::IntK)
					{
						::std::cout << "\tmovaps\txmm1, xmm0\n";
						::std::cout << "\tcvtsi2ss\txmm0, [rbp-"
							<< ptr1->offset
							<< "]\n";
						::std::cout << "\taddss\txmm0, xmm1\n";
						if ( it.getRes().type == IntK)
						{
							::std::cout << "\tcvttss2si\teax, xmm0\n";
						}
					}
				}
			} else if ( ptr1 == nullptr && ptr0 != nullptr)
			{
				if ( ptr0->nametype == 0) 
				{
					if ( it.getVal2().type == TypeKind::IntK)
					{
						::std::cout << "\tadd\teax, " << ptr0->name << ::std::endl;
						if ( it.getRes().type == FloatK)
						{
							::std::cout << "\tpxor\txmm0, xmm0\n";
							::std::cout << "\tcvtsi2ss\txmm0, eax\n";
						}
					} else if ( it.getVal2().type == TypeKind::FloatK)
					{
						bool flag = false;
						for ( auto x : literal)
						{
							if ( x == ptr1->name)
							{
								flag = true;
								break;
							}
						}
						if ( !flag)
						{
							literal.push_back(ptr0->name);
						}
						::std::cout << "\tmovaps\txmm1, xmm0\n";
						::std::cout << "\tmovss\txmm0, .L"
							<< ptr0->floatcounter
							<< "[rip]\n";
						::std::cout << "\taddss\txmm0, xmm1\n";
						if ( it.getRes().type == IntK)
						{
							::std::cout << "\tcvttss2si\teax, xmm0";
						}
					}
				} else if ( it.getVal2().type == TypeKind::IntK)
				{
					if ( it.getVal1().type == TypeKind::IntK)
					{
						::std::cout << "\tadd\teax, [rbp-"
							<< ptr0->offset
							<< "]\n";
						if ( it.getRes().type == TypeKind::FloatK)
						{
							::std::cout << "\tpxor\txmm0, xmm0\n";
							::std::cout << "\tcvtsi2ss\txmm0, eax\n";
						}
					} else if ( it.getVal1().type == TypeKind::FloatK)
					{
						if ( it.getRes().type == TypeKind::IntK)
						{
							::std::cout << "\tpxor\txmm0, xmm0\n";
							::std::cout << "\tcvtsi2ss\txmm0, eax\n";
							::std::cout << "\taddss\txmm0, DWORD PTR [rbp-"
								<< ptr0->offset
								<< "]\n";
							::std::cout << "\tcvttss2si\teax, xmm0\n";
						} else if ( it.getRes().type == TypeKind::FloatK)
						{
							::std::cout << "\tpxor\txmm0, xmm0\n";
							::std::cout << "\tcvtsi2ss\txmm0, eax\n";
							::std::cout << "\tmovss\txmm1, DWORD PTR [rbp-"
								<< ptr0->offset
								<< "]\n";
							::std::cout << "\taddss\txmm0, xmm1\n";
						}
					}
				} else if ( it.getVal2().type == TypeKind::FloatK)
				{
					if ( it.getVal1().type == TypeKind::FloatK)
					{
						::std::cout << "\taddss xmm0, DWORD PTR [rbp-"
							<< ptr0->offset
							<< "]\n";
						if ( it.getRes().type == IntK)
						{
							::std::cout << "\tcvttss2si\teax, xmm0\n";
						}
					} else if ( it.getVal1().type == TypeKind::IntK)
					{
						::std::cout << "\tmovaps\txmm1, xmm0\n";
						::std::cout << "\tcvtsi2ss\txmm0, DWORD PTR [rbp-"
							<< ptr0->offset
							<< "]\n";
						::std::cout << "\taddss\txmm0, xmm1\n";
						if ( it.getRes().type == IntK)
						{
							::std::cout << "\tcvttss2si\teax, xmm0\n";
						}
					}
				}
			} else 
			{
				if ( ptr0->nametype == 0 && ptr1->nametype == 0)
				{
					::std::stringstream ss;
					ss << ptr0->name;
					float tmp;
					float tmp2;
					ss >> tmp;
					ss.clear();
					ss << ptr1->name;
					ss >> tmp2;
					tmp = tmp + tmp2;

					ss.clear();
					ss << tmp;

					if ( it.getRes().type == IntK)
					{
						::std::cout << "\tmov\teax, " << ss.str() << ::std::endl;
					} else if ( it.getRes().type == FloatK)
					{
						bool flag = false;
						for ( auto it:literal)
						{
							if ( ptr0->name == it)
							{
								flag = true;
								break;
							}
						}
						if ( !flag )
						{
							literal.push_back(ptr0->name);
							flag = false;
						}
						for ( auto it : literal)
						{
							if ( ptr1->name == it)
							{
								flag = true;
								break;
							}
						}
						if ( !flag )
						{
							literal.push_back(ptr1->name);
						}
						::std::cout << "\tmovss\txmm0, L."
							<< ptr0->floatcounter
							<< "[rip]\n";
						::std::cout << "\tmovss\txmm1, L."
							<< ptr1->floatcounter
							<< "[rip]\n";
						::std::cout << "\taddss\txmm0, xmm1\n";
					}
				} else if ( ptr0->nametype != 0 && ptr1->nametype == 0)
				{
					if ( it.getVal2().type == IntK && it.getRes().type == IntK)
					{
						::std::stringstream ss;
						ss << ptr1->name;
						int tmp;
						ss >> tmp;
						::std::cout << "\tmov\teax, " << ss.str() << ::std::endl;
						::std::cout << "\tadd eax, DWORD PTR [rbp-"
							<< ptr0->offset
							<< "]\n";
					} else if ( it.getVal2().type == IntK && it.getRes().type == FloatK)
					{
						bool flag = false;
						for ( auto it:literal)
						{
							if ( ptr1->name == it)
							{
								flag = true;
								break;
							}
						}
						if ( !flag )
						{
							literal.push_back(ptr1->name);
							flag = false;
						}
						::std::cout << "pxor xmm0, xmm0\n";
						::std::cout << "\tcvtsi2ss\txmm0, DWORD PTR [rbp-"
							<< ptr0->offset
							<< "]\n";
						::std::cout << "\tmovss\txmm1, L."
							<< ptr1->floatcounter
							<< "[rip]\n";
						::std::cout << "\taddss\txmm0, xmm1\n";
					} else if ( it.getVal2().type == FloatK)
					{
						bool flag = false;
						for ( auto it:literal)
						{
							if ( ptr1->name == it)
							{
								flag = true;
								break;
							}
						}
						if ( !flag )
						{
							literal.push_back(ptr1->name);
							flag = false;
						}
						::std::cout << "\tmovss\txmm0, DWORD PTR [rbp-"
							<< ptr0->offset
							<< "]\n";
						::std::cout << "\tmovss\txmm1, L."
							<< ptr1->floatcounter
							<< "[rip]\n";
						::std::cout << "\taddss\txmm0, xmm1\n";
						if ( it.getRes().type == IntK)
						{
							::std::cout << "\tcvttss2si\teax, xmm0\n";
						}
					}
				} else if ( ptr1->nametype != 0 && ptr0->nametype == 0)
				{
					if ( it.getVal1().type == IntK && it.getRes().type == IntK)
					{
						::std::stringstream ss;
						ss << ptr0->name;
						int tmp;
						ss >> tmp;
						::std::cout << "\tmov\teax, " << ss.str() << ::std::endl;
						::std::cout << "\tadd eax, DWORD PTR [rbp-"
							<< ptr1->offset
							<< "]\n";
					} else if ( it.getVal1().type == IntK && it.getRes().type == FloatK)
					{
						bool flag = false;
						for ( auto it:literal)
						{
							if ( ptr0->name == it)
							{
								flag = true;
								break;
							}
						}
						if ( !flag )
						{
							literal.push_back(ptr0->name);
							flag = false;
						}
						::std::cout << "pxor xmm0, xmm0\n";
						::std::cout << "\tcvtsi2ss\txmm0, DWORD PTR [rbp-"
							<< ptr1->offset
							<< "]\n";
						::std::cout << "\tmovss\txmm1, L."
							<< ptr0->floatcounter
							<< "[rip]\n";
						::std::cout << "\taddss\txmm0, xmm1\n";
					} else if ( it.getVal1().type == FloatK)
					{
						bool flag = false;
						for ( auto it:literal)
						{
							if ( ptr0->name == it)
							{
								flag = true;
								break;
							}
						}
						if ( !flag )
						{
							literal.push_back(ptr0->name);
							flag = false;
						}
						::std::cout << "\tmovss\txmm0, DWORD PTR [rbp-"
							<< ptr1->offset
							<< "]\n";
						::std::cout << "\tmovss\txmm1, L."
							<< ptr0->floatcounter
							<< "[rip]\n";
						::std::cout << "\taddss\txmm0, xmm1\n";
						if ( it.getRes().type == IntK)
						{
							::std::cout << "\tcvttss2si\teax, xmm0\n";
						}
					}
				} else 
				{
					if ( it.getRes().type == IntK)
					{
						if ( it.getVal1().type == IntK && it.getVal2().type == IntK)
						{
							::std::cout << "\tmov\teax, DWORD PTR [rbp-"
								<< ptr0->offset
								<< "]\n";
							::std::cout << "\tadd\teax, DWORD PTR [rbp-"
								<< ptr1->offset
								<< "]\n";
						} else if ( it.getVal1().type == FloatK && it.getVal2().type == IntK)
						{
							::std::cout << "\tpxor\txmm0, xmm0\n";
							::std::cout << "\tcvtsi2ss\txmm0, DWORD PTR [rbp-"
								<< ptr1->offset
								<< "]\n";
							::std::cout << "\taddss\txmm0, DWORD PTR [rbp-"
								<< ptr0->offset
								<< "]\n";
							::std::cout << "\tcvttss2si\teax, xmm0\n";
						} else if ( it.getVal2().type == FloatK && it.getVal1().type == IntK)
						{
							::std::cout << "\tpxor\txmm0, xmm0\n";
							::std::cout << "\tcvtsi2ss xmm0, DWORD PTR [rbp-"
								<< ptr0->offset
								<< "]\n";
							::std::cout << "\taddss\txmm0, DWORD PTR [rbp-"
								<< ptr1->offset
								<< "]\n";
							::std::cout << "\tcvtss2si\teax, xmm0\n";
						} else
						{
							::std::cout << "\tmovss\txmm0, DWORD PTR [rbp-"
								<< ptr0->offset
								<< "]\n";
							::std::cout << "\taddss\txmm0, DWORD PTR [rbp-"
								<< ptr0->offset
								<< "]\n";
							::std::cout << "\tcvttss2si\teax, xmm0\n";
						}
					} else if ( it.getRes().type == FloatK)
					{
						if ( it.getVal1().type == IntK && it.getVal2().type == IntK)
						{
							::std::cout << "\tmov\teax, DWORD PTR [rbp-"
								<< ptr0->offset
								<< "]\n";
							::std::cout << "\tadd\teax, DWORD PTR [rbp-"
								<< ptr1->offset
								<< "]\n";
							::std::cout << "\tpxor\txmm0, xmm0\n";
							::std::cout << "\tcvtsi2ss xmm0, eax\n";
						} else if ( it.getVal1().type == FloatK && it.getVal2().type == IntK)
						{
							::std::cout << "\tpxor\txmm0, xmm0\n";
							::std::cout << "\tcvtsi2ss\txmm0, DWORD PTR [rbp-"
								<< ptr1->offset
								<< "]\n";
							::std::cout << "\taddss\txmm0, DWORD PTR [rbp-"
								<< ptr0->offset
								<< "]\n";
						} else if ( it.getVal2().type == FloatK && it.getVal1().type == IntK)
						{
							::std::cout << "\tpxor\txmm0, xmm0\n";
							::std::cout << "\tcvtsi2ss xmm0, DWORD PTR [rbp-"
								<< ptr0->offset
								<< "]\n";
							::std::cout << "\taddss\txmm0, DWORD PTR [rbp-"
								<< ptr1->offset
								<< "]\n";
						} else
						{
							::std::cout << "\tmovss\txmm0, DWORD PTR [rbp-"
								<< ptr0->offset
								<< "]\n";
							::std::cout << "\taddss\txmm0, DWORD PTR [rbp-"
								<< ptr0->offset
								<< "]\n";
						}
					}
				}

			}
		} else if (it.getOperand() == "-")
		{
			auto ptr1 = 
				now_tab->research_elem_global(it.getVal1().name);
			auto ptr2 =
				now_tab->research_elem_global(it.getVal2().name);

			if ( ptr1 == nullptr && ptr2 != nullptr)
			{
				if ( ptr2->nametype == 0)
				{
					bool flag = false;
					if ( it.getVal2().type == IntK)
					{
						if (it.getVal1().type == IntK)
						{
							::std::cout << "\tsub eax, " << ptr2->name << ::std::endl;
						} else if (it.getVal1().type == FloatK)
						{
							for(auto it : literal)
							{
								if ( it == ptr2->name)
								{
									flag = true;
									break;
								}
							}
							if ( !flag)
							{
								literal.push_back(ptr2->name);
							}
							::std::cout << "\tmovss\txmm1, .L"
								<< ptr2->floatcounter
								<< "[rip]\n";
							::std::cout << "\tsubss\txmm0, xmm1\n";
						}
					} else if (it.getVal2().type == FloatK)
					{
						if (it.getVal1().type == FloatK)
						{
							::std::cout << "\tmovss\txmm1, .L"
								<< ptr2->floatcounter
								<< "[rip]\n";
							::std::cout << "\tsubss\txmm0, xmm1\n";
						} else if ( it.getVal1().type == IntK)
						{
							::std::cout << "\tpxor\txmm0, xmm0\n";
							::std::cout << "\tcvtsi2ss\txmm0, eax\n";
							::std::cout << "\tmovss\txmm1, .L"
								<< ptr2->floatcounter
								<< "[rip]\n";
						}
					}
				} else if ( it.getVal1().type == IntK && it.getVal2().type == IntK)
				{
					::std::cout << "\tsub\teax, DWORD PTR [rbp-"
						<< ptr2->offset
						<< "]\n";
				} else if ( it.getVal1().type == FloatK && it.getVal2().type == IntK)
				{
					::std::cout << "\tpxor\txmm1, xmm1\n";
					::std::cout << "\tcvtsi2ss\txmm1, DWORD PTR [rbp-"
						<< ptr2->offset
						<< "]\n";
					::std::cout << "\tsubss\txmm0, xmm1\n";
				} else if ( it.getVal1().type == FloatK && it.getVal2().type == FloatK)
				{
					::std::cout << "\tsubss\txmm0, DWORD PTR [rbp"
						<< ptr2->offset
						<< "]\n";
				}
			} else if (ptr1 != nullptr && ptr2 == nullptr)
			{
				if ( ptr1->nametype == 0)
				{
					bool flag = false;
					if ( it.getVal1().type == IntK)
					{
						if (it.getVal2().type == IntK)
						{
							::std::cout << "\tsub eax, " << ptr1->name << ::std::endl;
						} else if (it.getVal2().type == FloatK)
						{
							for(auto it : literal)
							{
								if ( it == ptr1->name)
								{
									flag = true;
									break;
								}
							}
							if ( !flag)
							{
								literal.push_back(ptr1->name);
							}
							::std::cout << "\tmovss\txmm1, .L"
								<< ptr1->floatcounter
								<< "[rip]\n";
							::std::cout << "\tsubss\txmm0, xmm1\n";
						}
					} else if (it.getVal1().type == FloatK)
					{
						if (it.getVal2().type == FloatK)
						{
							::std::cout << "\tmovss\txmm1, .L"
								<< ptr1->floatcounter
								<< "[rip]\n";
							::std::cout << "\tsubss\txmm0, xmm1\n";
						} else if ( it.getVal2().type == IntK)
						{
							::std::cout << "\tpxor\txmm0, xmm0\n";
							::std::cout << "\tcvtsi2ss\txmm0, eax\n";
							::std::cout << "\tmovss\txmm1, .L"
								<< ptr1->floatcounter
								<< "[rip]\n";
						}
					}
				} else if ( it.getVal2().type == IntK && it.getVal1().type == IntK)
				{
					::std::cout << "\tsub\teax, DWORD PTR [rbp-"
						<< ptr1->offset
						<< "]\n";
				} else if ( it.getVal2().type == FloatK && it.getVal1().type == IntK)
				{
					::std::cout << "\tpxor\txmm1, xmm1\n";
					::std::cout << "\tcvtsi2ss\txmm1, DWORD PTR [rbp-"
						<< ptr1->offset
						<< "]\n";
					::std::cout << "\tsubss\txmm0, xmm1\n";
				} else if ( it.getVal2().type == FloatK && it.getVal1().type == FloatK)
				{
					::std::cout << "\tsubss\txmm0, DWORD PTR [rbp"
						<< ptr1->offset
						<< "]\n";
				}
			} else if ( ptr1 != nullptr && ptr2 != nullptr)
			{
				if ( ptr1->nametype == 0 && ptr2->nametype == 0)
				{
					if ( it.getVal1().type == IntK && it.getVal2().type == IntK)
					{
						::std::stringstream ss;
						int tmp1;
						int tmp2;
						ss << ptr1->name;
						ss >> tmp1;
						ss.clear();
						ss << ptr2->name;
						ss >> tmp2;

						::std::cout << "\tmov\teax, " << (tmp1-tmp2) << ::std::endl;
					} else if ( it.getVal1().type == FloatK || it.getVal2().type == FloatK)
					{
						bool flag = false;

						for ( auto it : literal)
						{
							if ( it == ptr1->name)
							{
								flag = true;
								break;
							}
						}
						if ( !flag )
						{
							literal.push_back(ptr1->name);
							flag = false;
						}
						for ( auto it : literal)
						{
							if ( it == ptr2->name)
							{
								flag = true;
								break;
							}
						}
						if ( !flag )
						{
							literal.push_back(ptr2->name);
							flag = false;
						}

						::std::cout << "\tmovss\txmm0, DWORD PTR .L"
							<< ptr1->floatcounter
							<< "[rip]\n";
						::std::cout << "\tsubss\txmm0, DWORD PTR .L"
							<< ptr2->floatcounter
							<< "[rip]\n";
					}
				} else if ( ptr1->nametype == 0 && ptr2->nametype != 0)
				{
					if ( it.getVal1().type == IntK && it.getVal2().type == IntK)
					{
						::std::cout << "\tmov\teax, " << ptr1->name << ::std::endl;
						::std::cout << "\tsub\teax, DWORD PTR [rbp-"
							<< ptr2->offset
							<< "]\n";
					} else if (it.getVal1().type == IntK && it.getVal2().type == FloatK)
					{
						::std::cout << "\tmovss\txmm0, DOWRD PTR .L"
							<< ptr1->floatcounter
							<< "[rip]\n";
						bool flag = false;
						for ( auto it : literal)
						{
							if ( it == ptr1->name)
							{
								flag = true;
								break;
							}
						}
						if ( !flag)
						{
							literal.push_back(ptr1->name);
						}
						::std::cout << "\tsubss\txmm0, DWORD PTR [rbp-"
							<< ptr2->offset
							<< "]\n";
					} else if ( it.getVal1().type == FloatK && it.getVal2().type == IntK)
					{
						bool flag = false;
						for ( auto it : literal)
						{
							if ( it == ptr1->name)
							{
								flag = true;
								break;
							}
						}

						if ( !flag)
						{
							literal.push_back(ptr1->name);
						}
						::std::cout << "\tmovss\txmm0, .L"
							<< ptr1->floatcounter
							<< "[rip]\n";
						::std::cout << "\tpxor\txmm1, xmm1\n";
						::std::cout << "\tcvtsi2ss\txmm1, DWORD PTR [rbp-"
							<< ptr2->offset
							<< "]\n";
						::std::cout << "\tsubss\txmm0, xmm1\n";
					} else 
					{
						bool flag = false;
						for ( auto it : literal)
						{
							if ( it == ptr1->name)
							{
								flag = true;
								break;
							}
						}
						if ( !flag)
						{
							literal.push_back(ptr1->name);
						}
						::std::cout << "\tmovss\txmm0, .L"
							<< ptr1->floatcounter
							<< "[rip]\n";
						::std::cout << "\tsubss\txmm0, DWORD PTR [rbp-"
							<< ptr2->offset
							<< "[rip]\n";
					}
				} else if ( ptr1->nametype != 0 && ptr2->nametype == 0)
				{
					if ( it.getVal2().type == IntK && it.getVal1().type == IntK)
					{
						::std::cout << "\tmov\teax, " << ptr2->name << ::std::endl;
						::std::cout << "\tsub\teax, DWORD PTR [rbp-"
							<< ptr1->offset
							<< "]\n";
					} else if (it.getVal2().type == IntK && it.getVal1().type == FloatK)
					{
						::std::cout << "\tmovss\txmm0, DOWRD PTR .L"
							<< ptr2->floatcounter
							<< "[rip]\n";
						bool flag = false;
						for ( auto it : literal)
						{
							if ( it == ptr2->name)
							{
								flag = true;
								break;
							}
						}
						if ( !flag)
						{
							literal.push_back(ptr2->name);
						}
						::std::cout << "\tsubss\txmm0, DWORD PTR [rbp-"
							<< ptr1->offset
							<< "]\n";
					} else if ( it.getVal2().type == FloatK && it.getVal1().type == IntK)
					{
						bool flag = false;
						for ( auto it : literal)
						{
							if ( it == ptr2->name)
							{
								flag = true;
								break;
							}
						}

						if ( !flag)
						{
							literal.push_back(ptr2->name);
						}
						::std::cout << "\tmovss\txmm0, .L"
							<< ptr2->floatcounter
							<< "[rip]\n";
						::std::cout << "\tpxor\txmm1, xmm1\n";
						::std::cout << "\tcvtsi2ss\txmm1, DWORD PTR [rbp-"
							<< ptr1->offset
							<< "]\n";
						::std::cout << "\tsubss\txmm0, xmm1\n";
					} else 
					{
						bool flag = false;
						for ( auto it : literal)
						{
							if ( it == ptr2->name)
							{
								flag = true;
								break;
							}
						}
						if ( !flag)
						{
							literal.push_back(ptr2->name);
						}
						::std::cout << "\tmovss\txmm0, .L"
							<< ptr2->floatcounter
							<< "[rip]\n";
						::std::cout << "\tsubss\txmm0, DWORD PTR [rbp-"
							<< ptr1->offset
							<< "[rip]\n";
					}

				} else if ( ptr1->nametype != 0 && ptr2->nametype != 0)
				{
					if ( it.getVal1().type == IntK && it.getVal2().type == IntK)
					{
						::std::cout << "\tmov\teax, DWORD PTR [rbp-"
							<< ptr1->offset
							<< "]\n";
						::std::cout << "\tsub\teax, DWORD PTR [rbp-"
							<< ptr2->offset
							<< "]\n";
					} else if (it.getVal1().type == FloatK && it.getVal2().type == FloatK)
					{
						::std::cout << "\tmovss\txmm0, DWORD PTR [rbp-"
							<< ptr1->offset
							<< "]\n";
						::std::cout << "\tsubss\txmm0, DWORD PTR [rbp-"
							<< ptr2->offset;
					} else if ( it.getVal1().type == IntK && it.getVal2().type == FloatK)
					{
						::std::cout << "\tpxor\txmm0, xmm0\n";
						::std::cout << "\tcvtsi2ss\txmm0, DWORD PTR [rbp-"
							<< ptr1->offset
							<< "]\n";
						::std::cout << "\tsubss\txmm0, DWORD PTR [rbp-"
							<< ptr2->offset
							<< "]\n";
					} else if ( it.getVal1().type == FloatK && it.getVal2().type == IntK)
					{
						::std::cout << "\tmovss\txmm0, DWORD PTR [rbp-"
							<< ptr1->offset
							<< "]\n";
						::std::cout << "\tpxor\txmm1, xmm1\n";
						::std::cout << "\tcvtsi2ss\txmm1, DWORD PTR [rbp-"
							<< ptr2->offset
							<< "]\n";
						::std::cout << "\tsubss\txmm0, xmm1\n";
					}
				}
			}

		} else if ( it.getOperand() == "=")
		{
			::std::cout << "7\n";
			if ( it.getRes().name.at(0) != '@')
			{
				auto ptr0 =
					now_tab->research_elem_global(it.getRes().name);
				auto ptr1 = 
					now_tab->research_elem_global(it.getVal1().name);

				if ( it.getRes().type == TypeKind::FloatK 
						&& it.getVal1().name.at(0) == '@')
				{
					if ( it.getVal1().type == FloatK)
					{
						::std::cout << "\tmovss\tDWORD PTR [rbp-"
							<< ptr0->offset
							<< "], xmm0\n";
					} else if (it.getVal1().type == IntK)
					{
						::std::cout << "\tpxor\txmm0, xmm0\n";
						::std::cout << "\tcvtsi2ss\txmm0, eax\n";
						::std::cout << "\tmovss\tDWORD PTR [rbp-"
							<< ptr0->offset
							<< "], xmm0\n";
					}
				} else if (it.getRes().type == TypeKind::IntK 
						&& it.getVal1().name.at(0) == '@')
				{
					if ( it.getVal1().type == IntK)
					{
						::std::cout << "\tmov DWORD PTR [rbp-"
							<< ptr0->offset
							<< "], eax\n";
					} else if ( it.getVal1().type == FloatK)
					{
						::std::cout << "\tcvttss2si\teax, xmm0\n";
						::std::cout << "\tmov\tDWORD PTR [rbp-"
							<< ptr0->offset
							<< "], eax\n";
					}
				} else if ( ptr1->nametype != 0)
				{
					if ( it.getRes().type == TypeKind::IntK)
					{
						if ( it.getVal1().type == TypeKind::IntK)
						{
							::std::cout << "\tmov\teax, DWORD PTR [rbp-"
								<< ptr1->offset
								<< "]\n";
							::std::cout << "\tmov\tDWORD PTR [rbp-"
								<< ptr0->offset
								<< "], eax\n";
						} else if ( it.getVal1().type == TypeKind::FloatK)
						{
							::std::cout << "\tmovss\t xmm0, DWORD PTR [rbp-"
								<< ptr1->offset
								<< "]\n";
							::std::cout << "\tcvtss2si\teax, xmm0\n";
							::std::cout << "\tmov DWORD PTR [rbp-\t"
								<< ptr0->offset
								<< "], eax\n";
						}
					} else if ( it.getRes().type == TypeKind::FloatK)
					{
						if ( it.getVal1().type == TypeKind::FloatK)
						{
							::std::cout << "\tmovss\txmm0, DWORD PTR [rbp-"
								<< ptr1->offset
								<< "]\n";
							::std::cout << "\tmovss\tDWORD PTR [rbp-"
								<< ptr0->offset
								<< "], xmm0\n";
						} else if ( it.getVal1().type == TypeKind::IntK)
						{
							::std::cout << "\tpxor\txmm0, xmm0\n";
							::std::cout << "\tcvtsi2ss\txmm0, DWORD PTR [rbp-"
								<< ptr1->offset
								<< "]\n";
							::std::cout << "\tmovss DWORD PTR [rbp-"
								<< ptr0->offset
								<< "], xmm0\n";
						}
					} 
				} else 
				{
					if ( it.getRes().type == TypeKind::FloatK)
					{
						bool flag = false;
						for ( auto x : literal)
						{
							if ( x == it.getVal1().name)
							{
								flag = true;
								break;
							}
						}
						if ( !flag )
						{
							literal.push_back(it.getVal1().name);
						}
						auto ptr = current_tab->research_elem_global(it.getVal1().name);
						::std::cout << "\tmovss\txmm0, .L"
							<< ptr->floatcounter
							<< "[rip]\n";
						::std::cout << "\tmovss\tDWORD PTR [rbp-"
							<< ptr0->offset
							<< "], xmm0\n";
					} else if ( it.getRes().type == TypeKind::IntK)
					{
						::std::stringstream ss;
						ss << it.getVal1().name;
						int tmp;
						ss >> tmp;

						::std::cout << "\tmov\teax, " << tmp << ::std::endl;
						::std::cout << "\tmov\tDWORD PTR [rbp-"
							<< ptr0->offset
							<< "], eax\n";
					}
				}
			} 
		} else if ( it.getOperand() == "return")
		{
			::std::cout << "8\n";
			if ( it.getVal1().name.at(0) != '@')
			{
				auto ptr = 
					now_tab->research_elem_global(it.getVal1().name);
				auto func = current_tab->research_elem_global(now_tab->get_in_function());
				if ( ptr->nametype != 0)
				{
					if ( func->datatype == TypeKind::IntK)
					{
						if ( ptr->datatype == TypeKind::IntK)
						{
							::std::cout << "\tmov\teax, DWORD PTR [rbp-" 
								<< ptr->offset
								<< "]\n";
						} else 
						{
							::std::cout << "\tmovss\txmm0, [rbp-"
								<< ptr->offset
								<< "]\n";
							::std::cout << "\tcvttss2si\teax, xmm0\n";
						}
					} else 
					{
						if ( ptr->datatype == TypeKind::FloatK)
						{
							::std::cout << "\tmovss\txmm0, DWORD PTR [rbp-"
								<< ptr->offset
								<< "]\n";
						} else 
						{
							::std::cout << "\tpxor\txmm0, xmm0\n";
							::std::cout << "\tcvtsi2ss\txmm0, DWORD PTR [rbp-"
								<< ptr->offset
								<< "]\n";
						}
					}

				} else 
				{
					if ( it.getVal1().type == TypeKind::IntK)
					{
						::std::cout << "\tmov\teax, "
							<<  ptr->name << ::std::endl;
					} else 
					{
						bool flag = false;
						for ( auto x : literal)
						{
							if ( x == it.getVal1().name)
							{
								flag = true;
								break;
							}
						}
						if ( !flag )
						{
							literal.push_back(it.getVal1().name);
						}
						auto ptr = current_tab->research_elem_global(it.getVal1().name);
						::std::cout << "\tmovss\txmm0, DWORD PTR "
							<< ".L" << ptr->floatcounter
							<< "[rip]\n";
					}
				}
			}
		} else 
		{
			::std::cout << "9\n";
			continue;
		}
	}

	::std::cout << "\t.section\t.rodata\n";
	::std::cout << "\t.align 4\n";

	for(auto x : literal)
	{
		auto ptr = current_tab->research_elem(x);
		::std::cout << ".L" << ptr->floatcounter << ":\n";

		::std::stringstream ss;
		int s;
		float* tmpfloat = (float *)&s;
		ss << x;
		ss >> *tmpfloat;
		::std::cout << "\t.long\t" << s << ::std::endl;
		::std::cout << "\t.align\t4\n";
	}
}
}
