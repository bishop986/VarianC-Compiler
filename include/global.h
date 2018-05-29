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
	BOOLEAN = 8, VOID, INT, FLOAT, BREAK, CONTINUE, ELSE, FOR, IF, RETURN, WHILE, ADDOP, MULOP, RELOP, EQUOP, ASSINGOP, LOGICALOP, NONES, STRING, ID, SEPOP, NUMBOOLEAN, NUMINT, NUMFLOAT
};

enum NodeKind{
	DeclK, ExprK, DeclaratorK, InitialiserK, StmtK
};

enum DeclKind{
	FuncK, VarK, ParaK
};

enum DeclaratorKind{
	PrimitiveK, ArrayK
};

enum InitialiserKind{
	ListK, SingleK
};

enum ExprKind{
	AssignK, CondOrK, CondAndK, EquK, RelK, AdditiveK, MulK, UnaryK, ArrayExp, FuncExp, BraketExp, INTLITERAL, FLOATLITERAL, BOOLLITERAL, STRINGLITERAL, IdK
};

enum StmtKind{
	ComK, IfK, ForK, WhileK, BreakK, ContinueK, RetK, ExpK
};

enum TypeKind{
	BoolK, StringK, IntK, FloatK
};

}
#endif
