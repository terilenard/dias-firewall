/*
* This work is licensed under the terms of the MIT license.  
* For a copy, see <https://opensource.org/licenses/MIT>.
*
* Developed by NISLAB - Network and Information Security Laboratory
* at George Emil Palade University of Medicine, Pharmacy, Science and
* Technology of Târgu Mureş <https://nislab.umfst.ro/>
*
* Contributors: Bela Genge
*/

#pragma once
#include <list>
#include <string>
using namespace std;
class ExprValue;

class ExpressionTree
{
public:
	typedef enum {
		OP_AND,
		OP_OR
	}OP_TYPE;

private:
	typedef struct expression{
		OP_TYPE opType;
		list<expression*> lstExpr;
		list<ExprValue*> lstValues;
	}EXPRESSION;

public:
	ExpressionTree();
	virtual ~ExpressionTree();

	bool operatorStart(const string& type);
	void operatorEnd(void);
	void addSingleValue(const string& index, const string& value);
	void addBoundValues(const string& index, const string& bounds);

	void printTree(void);

	bool matchMessage(const unsigned char* pPayload, const int nPayloadSz);

private:
	void recursiveCleanup(EXPRESSION* pExpr);
	void recursivePrint(EXPRESSION* pExpr, int nTabs);
	bool recursiveMatch(EXPRESSION* pExpr, const unsigned char* pPayload, const int nPayloadSz);
	bool matchAND(list<ExprValue*>& lstValues, const unsigned char* pPayload, const int nPayloadSz);
	bool matchOR(list<ExprValue*>& lstValues, const unsigned char* pPayload, const int nPayloadSz);

private:
	EXPRESSION* m_pRootExpr;
	EXPRESSION* m_pCurrentExpr;
	list<EXPRESSION*> m_lstExprStack;
};

