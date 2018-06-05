/*
 * Author: Dh
 * update Date: 2018.5.29
 * Update info: 
 * bug in INFLOAT, lastch is num, cur is num 
 * Fixed: if ( cur >= '0' && lastch <= '9') => if ( cur >= '0' && cur <= '9')
 * Fixed: number type INT > NUMINT
 */

#include "../include/scan.h"
#include "../include/global.h"
#include <fstream>

namespace dh{

scanner::scanner()
{
	_fp = NULL;
	scanflag = false;
	rightflag = true;
	_tokens.clear();
	line = 1;
}

void scanner::reset()
{
	destroy();
}

void scanner::destroy()
{
	if ( _fp != NULL)
	{
		::std::fclose(NULL);
		_fp = NULL;
	}
	_tokens.clear();
}

bool scanner::isScanned()
{
	return scanflag;
}

bool scanner::isRight()
{
	return rightflag;
}

scanner& scanner::operator=(const scanner& eq)
{
	this->_tokens.assign( eq._tokens.begin(), eq._tokens.end());
	this->_fp = eq._fp;
	this->scanflag = eq.scanflag;
	_it = _tokens.begin();;

	return *this;
}

void scanner::setBegin()
{
	_it = _tokens.begin();
}

token scanner::getToken()
{
	if ( scanflag == false || _tokens.end() == _it)
	{
		return token( "", TYPE::NONES, 0);
	}

	// use copy constructrue function
	token tmp(*_it);

	// pointer point to next word
	++_it;

	return tmp;
}

bool scanner::ungetToken()
{
	if ( scanflag == false || _tokens.begin() == _it)
	{
		return false;
	}

	// back to previous word
	--_it;
	return true;
}

/*
 * Transform Function
 */
bool scanner::scan(::std::FILE *fp)
{
	this->reset();

	STATE state = STATE::START;
	char cur = 0;
	char lastch = 0;
	::std::string tmp = "";

	if ( fp == NULL)
	{
		return false;
	}

	cur = ::std::fgetc(fp);
	while( cur == ' ' || cur == '\t') cur = ::std::fgetc(fp);

	// DFA implement
	while( cur != EOF)
	{
		if ( cur == '\n' && lastch != '\r')
		{
			++line;
		} else if ( cur == '\r')
		{
			++line;
		}

		// core part
		switch(state)
		{
			case STATE::START:
				if ( cur == ' ' || cur == '\t'
						|| cur == '\r' || cur == '\n')
				{
					state = STATE::START;
				} else if ( cur == '_' 
						|| ( cur >= 'a' && cur <= 'z')
						|| ( cur >= 'A' && cur <= 'Z'))
				{
					state = STATE::INID;
					tmp += cur;
				} else if ( cur >= '0' && cur <= '9')
				{
					state = STATE::ININT;
					tmp += cur;
				} else if ( cur == '\"') 
				{
					state = STATE::INSTRING;
					tmp += cur;
				} else if ( cur == '.') 
				{
					state = STATE::INFLOAT;
					tmp += cur;
				}
				else 
				{
					state = STATE::DONE;
				}
				lastch = cur;
				break;
			case INID:
				if ( cur == '_'
						|| ( cur >= 'a' && cur <= 'z')
						|| ( cur >= 'A' && cur <= 'Z')
						|| ( cur >= '0' && cur <= '9'))
				{
					state = STATE::INID;
					tmp += cur;
				} else 
				{
					state = STATE::DONE;
					
					TYPE tmp_type = TYPE::ID;
					if ( tmp == "boolean")
					{
						tmp_type = TYPE::BOOLEAN;
					} else if ( tmp == "break")
					{
						tmp_type = TYPE::BREAK;
					} else if ( tmp == "continue")
					{
						tmp_type = TYPE::CONTINUE;
					} else if ( tmp == "else")
					{
						tmp_type = TYPE::ELSE;
					} else if ( tmp == "for")
					{
						tmp_type = TYPE::FOR;
					} else if ( tmp == "float")
					{
						tmp_type = TYPE::FLOAT;
					} else if ( tmp == "if")
					{
						tmp_type = TYPE::IF;
					} else if ( tmp == "int")
					{
						tmp_type = TYPE::INT;
					} else if ( tmp == "return")
					{
						tmp_type = TYPE::RETURN;
					} else if ( tmp == "void")
					{
						tmp_type = TYPE::VOID;
					} else if ( tmp == "while")
					{
						tmp_type = TYPE::WHILE;
					} else if ( tmp == "true" || tmp == "false")
					{
						tmp_type = TYPE::NUMBOOLEAN;
					} else 
					{
						tmp_type = TYPE::ID;
					}
					_tokens.push_back( token( tmp, tmp_type, line));
					tmp.clear();
				}
				lastch = cur;
				break;
			case STATE::ININT:
				if ( cur >= '0' && cur <= '9')
				{
					tmp += cur;
					state = STATE::ININT;
				} else if ( cur == '.')
				{
					state = STATE::INFLOAT;
					tmp += cur;
				} else if ( cur == 'E' || cur == 'e')
				{
					tmp += cur;
					state = STATE::INFLOAT;
				} else 
				{
					state = STATE::DONE;
					
					_tokens.push_back( token( tmp, TYPE::NUMINT, line));
					tmp.clear();
				}
				lastch = cur;
				break;
			case STATE::INFLOAT:
				if ( lastch == '.')
				{
					if ( cur >= '0' && cur <= '9')
					{
						tmp += cur;
						state = STATE::INFLOAT;
					} else 
					{
						state = STATE::DONE;

						_tokens.push_back( token( tmp, TYPE::NUMFLOAT, line));
						tmp.clear();
					}
				} else if ( lastch == 'E' || lastch == 'e')
				{
					if ( cur == '+' || cur == '-')
					{
						state = STATE::INFLOAT;
						tmp += cur;
					} else if ( cur >= '0' && cur <= '9')
					{
						state = STATE::INFLOAT;
						tmp += cur;
					} else 
					{
						state = STATE::START;
						::std::cerr << "[ERROR] Incompleted float-literal \"" 
							<< tmp
							<< "\" near line " 
							<< line << ::std::endl;
						tmp.clear();
						rightflag = false;
					}
				} else if( lastch >= '0' && lastch <= '9')
				{
					if ( cur == 'E' || cur == 'e')
					{
						tmp += cur;
						state = STATE::INFLOAT;
					} else if ( cur >= '0' && cur <= '9')
					{
						tmp += cur;
						state = STATE::INFLOAT;
					} else
					{
						state = STATE::DONE;
						_tokens.push_back( token( tmp, TYPE::NUMFLOAT, line));
						tmp.clear();
					}
				} else if ( lastch == '+' || lastch == '-')
				{
					if ( cur >= '0' && cur <= '9')
					{
						tmp += cur;
						state = STATE::INFLOAT;
					} else 
					{
						::std::cerr << "[ERROR] Incompleted float-literal \""
							<< tmp
							<< "\" near line "
							<< line
							<< ::std::endl;
						tmp.clear();
						rightflag = false;
						state = STATE::START;
					}
				}
				lastch = cur;
				break;
			case STATE::INMULTICOMMENT:
				if ( lastch == '*' && cur == '/')
				{
					state = STATE::START;
				} else 
				{
					state = STATE::INMULTICOMMENT;
				}
				lastch = cur;
				break;
			case STATE::INSINGLECOMMENT:
				if (cur == '\r' || cur == '\n')
				{
					state = STATE::START;
				} else 
				{
					state = STATE::INSINGLECOMMENT;
				}
				lastch = cur;
				break;
			case STATE::INSTRING:
				if ( cur == '\"' && lastch != '\\')
				{
					state = STATE::START;

					_tokens.push_back( token( tmp, TYPE::STRING, line));
					tmp.clear();
				} else 
				{
					state = STATE::INSTRING;
					if ( cur != '\n' && cur != '\r')
					{
						tmp += cur;
					}
				}
				lastch = cur;
				break;
			case STATE::DONE:
				switch(lastch)
				{
					case ' ':
					case '\t':
					case '\r':
					case '\n':
						break;
					case '=':
						if ( cur == '=')
						{
							tmp.clear();
							tmp = lastch + cur;

							_tokens.push_back( token( tmp, TYPE::EQUOP, line));
							tmp.clear();

							cur = ::std::fgetc(fp);
						} else 
						{
							tmp.clear();
							tmp = lastch;

							_tokens.push_back( token( tmp, TYPE::ASSINGOP, line));
							tmp.clear();
						}
						break;
					case '!':
						if ( cur == '=')
						{
							tmp.clear();
							tmp = "!=";

							_tokens.push_back( token( tmp, TYPE::EQUOP, line));
							tmp.clear();

							cur = ::std::fgetc(fp);
						} else 
						{
							tmp.clear();
							tmp = lastch;

							_tokens.push_back( token( tmp, TYPE::LOGICALOP, line));
							tmp.clear();
						}
						break;
					case '>':
					case '<':
						if ( cur == '=')
						{
							tmp.clear();
							tmp = lastch + cur;

							_tokens.push_back( token( tmp, TYPE::RELOP, line));
							tmp.clear();

							cur = ::std::fgetc(fp);
						} else 
						{
							tmp.clear();
							tmp = lastch;

							_tokens.push_back( token( tmp, TYPE::RELOP, line));
							tmp.clear();
						}
						break;
					case '|':
						if ( cur == '|')
						{
							tmp.clear();
							tmp = "||";

							_tokens.push_back( token( tmp, TYPE::LOGICALOP, line));
							tmp.clear();

							cur = ::std::fgetc(fp);
						} else 
						{
							tmp.clear();
							::std::cerr << "[ERROR] Incompleted Logical operator \"|\""
								<< " near line " 
								<< line
								<< ::std::endl;
							rightflag = false;
						}
						break;
					case '&':
						if ( cur == '&')
						{
							tmp.clear();
							tmp = "&&";

							_tokens.push_back( token( tmp, TYPE::LOGICALOP, line));
							tmp.clear();

							cur = ::std::fgetc(fp);
						} else 
						{
							tmp.clear();
							::std::cerr << "[ERROR] Incompleted Logical operator \"&\""
								<< " near line " 
								<< line
								<< ::std::endl;
							rightflag = false;
						}
						break;
					case '/':
						if ( cur == '/')
						{
							state = STATE::INSINGLECOMMENT;
							continue;
						} else if ( cur == '*'){
							state = STATE::INMULTICOMMENT;
							continue;
						}
					case '*':
						tmp.clear();
						tmp = lastch;
						_tokens.push_back( token( tmp, TYPE::MULOP, line));
						tmp.clear();
						break;
					case '+':
					case '-':
						tmp.clear();
						tmp = lastch;
						_tokens.push_back( token( tmp, TYPE::ADDOP, line));
						tmp.clear();
						break;
					case '{':
					case '}':
					case '[':
					case ']':
					case '(':
					case ')':
					case ',':
					case ';':
						tmp.clear();
						tmp = lastch;
						_tokens.push_back( token( tmp, TYPE::SEPOP, line));
						tmp.clear();
						break;
					default:
						::std::cerr << "[ERROR] Unexpected Token \""
							<< lastch
							<< "\" in line "
							<< line
							<< ::std::endl;
						rightflag = false;
						break;
				}
				if (cur == '\n' || cur == '\r')
				{
					--line;
				}
				state = STATE::START;
				continue;
		}
		cur = ::std::fgetc(fp);
	}
	_it = _tokens.begin();
	scanflag = true;;
	fclose(fp);
	return true;
}

void scanner::debug()
{
	if ( !scanflag)
	{
		::std::cerr << "[ERROR] Token Sequence is not ready\n";
		return;
	}

	::std::cout << "[INFO] Token Sries: " << ::std::endl;
	::std::cout << "[INFO] Size: " << _tokens.size() << ::std::endl;

	for(auto i = _tokens.begin(); i != _tokens.end(); ++i)
	{
		::std::string tmp_type;
		switch(i->getType())
		{
			case TYPE::BOOLEAN:
				tmp_type = "BOOLEAN";
				break;
			case TYPE::BREAK:
				tmp_type = "BREAK";
				break;
			case TYPE::CONTINUE:
				tmp_type = "CONTINUE";
				break;
			case TYPE::ELSE:
				tmp_type = "ELSE";
				break;
			case TYPE::FOR:
				tmp_type = "FOR";
				break;
			case TYPE::FLOAT:
				tmp_type = "FLOAT";
				break;
			case TYPE::IF:
				tmp_type = "IF";
				break;
			case TYPE::INT:
				tmp_type = "INT";
				break;
			case TYPE::RETURN:
				tmp_type = "RETURN";
				break;
			case TYPE::VOID:
				tmp_type = "VOID";
				break;
			case TYPE::WHILE:
				tmp_type = "WHILE";
				break;
			case TYPE::ADDOP:
				tmp_type = "ADDOP";
				break;
			case TYPE::MULOP:
				tmp_type = "MULOP";
				break;
			case TYPE::RELOP:
				tmp_type = "RELOP";
				break;
			case TYPE::EQUOP:
				tmp_type = "EQUOP";
				break;
			case TYPE::LOGICALOP:
				tmp_type = "LOGICALOP";
				break;
			case TYPE::ASSINGOP:
				tmp_type = "ASSINGOP";
				break;
			case TYPE::ID:
				tmp_type = "ID";
				break;
			case TYPE::SEPOP:
				tmp_type = "SEPOP";
				break;
			case TYPE::STRING:
				tmp_type = "STRING";
				break;
			case TYPE::NUMINT:
				tmp_type = "NUMINT";
				break;
			case TYPE::NUMFLOAT:
				tmp_type = "NUMFLOAT";
				break;
			case TYPE::NUMBOOLEAN:
				tmp_type = "NUMBOOLEAN";
			case TYPE::NONES:
				break;
		}
		::std::cout << "{\t" << i->getVal() << ",\t" << tmp_type << ::std::endl;
	}

	::std::cout << "[INFO] Token End" << ::std::endl;
}

}
