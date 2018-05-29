
/*
 * Author: Dh
 * Done Date: 2018.5.21
 * Introduction: token class implement
 */

#include "../include/global.h"
#include "../include/scan.h"

namespace dh {

token::token( ::std::string val, TYPE type, int lineno)
{
	this->_val = val;
	this->_type = type;
	this->_lineno = lineno;
}

token::token( const token& other)
{
	this->_val = other._val;
	this->_type = other._type;
	this->_lineno = other._lineno;
}

TYPE token::getType() const
{
	return _type;
}

::std::string token::getVal() const
{
	return _val;
}

int token::getLineno() const
{
	return _lineno;
}

token& token::operator=(const token& eq)
{
	this->_type = eq._type;
	this->_val = eq._val;
	this->_lineno = eq._lineno;
	return *this;
}

}
