#ifndef _GLOBAL_H_
#define _GLOBAL_H_
/*
 * Author: Dh
 * Update Date: 2018.5.21
 * Update info: 
 * add STATE to support DFA
 * add TYPE to support token type
 */


namespace dh {

/*********State********/
enum STATE {
	START = 0, INID, ININT, INFLOAT, INSTRING, DONE, INMULTICOMMENT, INSINGLECOMMENT 
};

/*********Token Type**********/
enum TYPE {
	BOOLEAN = 8, VOID, INT, FLOAT, BREAK, CONTINUE, ELSE, FOR, IF, RETURN, WHILE, ARITHOP, RELOP, EQUOP, ASSINGOP, LOGICALOP, NONES, STRING, ID, SEPOP
};

enum NodeKind{
	DeclK, ExpK, StmtK
};

enum DeclKind{
	FuncK, VarK
};

enum ExpKind{
	IdK, ConstK, OpK
};

enum StmtKind{
	ComK, IfK, ForK, WhileK, BreakK, ContinueK, RetK, AssignK
};

}
#endif
