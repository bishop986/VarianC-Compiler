#ifndef _SCAN_H_
#define _SCAN_H_

#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include "global.h"

namespace dh{

// token with string val and line no.
class token{
	public:

		token( ::std::string val, TYPE type, int lineno);
		token( const token& other);
		TYPE getType() const;
		::std::string getVal() const;
		int getLineno() const;
		token& operator=(const token& eq);
	private:

		::std::string _val;
		TYPE _type;
		int _lineno;
};

// scanner read characters from file
class scanner{
	public:

		void debug();
		scanner();

		// scanner generate token sequence from file
		bool scan( ::std::FILE *fp);
		// get next token from sequence
		token getToken();
		// walk back one step
		bool ungetToken();
		// clear token sequence
		void reset();
		~scanner() { destroy(); }
		scanner& operator=(const scanner& eq);
		// set sequence pointer to begin
		void setBegin();
		// check wether token is empty
		bool isScanned();
		// check is there a word error
		bool isRight();
	private:

		bool scanflag;
		bool rightflag;
		::std::FILE *_fp;
		::std::vector< token > _tokens;
		::std::vector< token >::const_iterator _it;
		int line;
		void destroy();
};

}

#endif
