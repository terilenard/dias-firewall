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

#include "ExpressionTree.h"
#include "ExprValue.h"
#include <cstring>

#ifndef _WIN32
#define strncpy_s(dest, destsz, src, count) strncpy((dest), (src), (count))
#endif

ExpressionTree::ExpressionTree() :
	m_pCurrentExpr(new EXPRESSION)
{
	m_pCurrentExpr->opType = OP_AND;
	m_pRootExpr = m_pCurrentExpr;
}

ExpressionTree::~ExpressionTree()
{
	recursiveCleanup(m_pRootExpr);
	delete m_pRootExpr;
}

bool ExpressionTree::operatorStart(const string& type)
{
	OP_TYPE opType;
	if (!type.compare("AND")) {
		opType = OP_AND;
	}
	else if (!type.compare("OR")) {
		opType = OP_OR;
	}
	else {
		return false;
	}

	// Create the new expression structure and add it to the main tree
	EXPRESSION* pex = new EXPRESSION;
	pex->opType = opType;
	m_pCurrentExpr->lstExpr.push_back(pex);

	// Save the current expression pointer and set the pointer to the current expression
	m_lstExprStack.push_front(m_pCurrentExpr);
	m_pCurrentExpr = pex;
	return true;
}

void ExpressionTree::operatorEnd(void)
{
	// Extract the expression from the stack - if any - and set it as the current one
	if (m_lstExprStack.size() <= 0) {
		return;
	}
	m_pCurrentExpr = m_lstExprStack.front();
	m_lstExprStack.pop_front();
}

void ExpressionTree::addSingleValue(const string& index, const string& value)
{
	int idx = stoi(index);
	int ivalue = stoi(value);

	ExprValue* exprValue = new ExprValue();
	exprValue->setValue(idx, ivalue);

	m_pCurrentExpr->lstValues.push_back(exprValue);
}

void ExpressionTree::addBoundValues(const string& index, const string& bounds)
{
	int idx = stoi(index);

	int nSize = bounds.size();
	char* pBuff = new char[nSize + 1];
	strncpy_s(pBuff, nSize+1, bounds.c_str(), nSize);
	pBuff[nSize] = '\0';

	char* pBoundLeft = pBuff;
	while (*pBoundLeft != '\0' && *pBoundLeft != '.') ++pBoundLeft;
	if (*pBoundLeft == '\0') {
		printf("Something went wrong, bounds are not properly formatted!\n");
		return;
	}
	*pBoundLeft = '\0';
	int iBoundLeft = stoi(pBuff);
	int iBoundRight = stoi((pBoundLeft+2));

	ExprValue* exprValue = new ExprValue();
	exprValue->setValue(idx, iBoundLeft, iBoundRight);

	m_pCurrentExpr->lstValues.push_back(exprValue);
}


void ExpressionTree::recursiveCleanup(EXPRESSION* pExpr)
{
	if (NULL == pExpr) {
		return;
	}

	// Cleanup value objects
	list<ExprValue*>::iterator itv;
	for (itv = pExpr->lstValues.begin(); itv != pExpr->lstValues.end(); ++itv) {
		delete (*itv);
	}

	// Recursively step through the sub-expressions
	list<EXPRESSION*>::iterator ite;
	for (ite = pExpr->lstExpr.begin(); ite != pExpr->lstExpr.end(); ++ite) {
		recursiveCleanup(*ite);
		delete (*ite);
	}
}

void ExpressionTree::printTree(void)
{
	recursivePrint(m_pRootExpr, 1);
}

void ExpressionTree::recursivePrint(EXPRESSION* pExpr, int nTabs)
{
	string sTabs = "";
	for (int i = 0; i < nTabs; ++i) {
		sTabs += "\t";
	}

	printf(sTabs.c_str());
	printf("Operator type: %s\n", (pExpr->opType==OP_AND)? "AND":"OR");
	sTabs += "\t";

	list<ExprValue*>::iterator it;
	for (it = pExpr->lstValues.begin(); it != pExpr->lstValues.end(); ++it) {
		printf(sTabs.c_str());
		printf("Byte ");
		(*it)->printValue();
		printf("\n");
	}

	list<EXPRESSION*>::iterator it1;
	for (it1 = pExpr->lstExpr.begin(); it1 != pExpr->lstExpr.end(); ++it1) {
		recursivePrint(*it1, nTabs + 1);
	}
}

bool ExpressionTree::matchMessage(const unsigned char* pPayload, const int nPayloadSz)
{
	return recursiveMatch(m_pRootExpr, pPayload, nPayloadSz);
}

bool ExpressionTree::recursiveMatch(EXPRESSION* pExpr, const unsigned char* pPayload, const int nPayloadSz)
{
	if (NULL == pExpr) { // This should not happen
		return false;
	}

	// AND
	if (OP_AND == pExpr->opType) {
		if (!matchAND(pExpr->lstValues, pPayload, nPayloadSz)) {
			return false;
		}
		list<EXPRESSION*>::iterator it;
		for (it = pExpr->lstExpr.begin(); it != pExpr->lstExpr.end(); ++it) {
			if (!recursiveMatch((*it), pPayload, nPayloadSz)) {
				return false;
			}
		}
		return true;
	}

	// OR
	if (matchOR(pExpr->lstValues, pPayload, nPayloadSz)) {
		// We do not neet to look any further...
		return true;
	}
	list<EXPRESSION*>::iterator it;
	for (it = pExpr->lstExpr.begin(); it != pExpr->lstExpr.end(); ++it) {
		if (recursiveMatch((*it), pPayload, nPayloadSz)) {
			// We do not neet to look any further...
			return true;
		}
	}
	
	return false;
}

bool ExpressionTree::matchAND(list<ExprValue*>& lstValues, const unsigned char* pPayload, const int nPayloadSz)
{
	list<ExprValue*>::iterator it;
	for (it = lstValues.begin(); it != lstValues.end(); ++it) {
		if (!(*it)->matchValue(pPayload, nPayloadSz)) {
			return false;
		}
	}
	return true;
}

bool ExpressionTree::matchOR(list<ExprValue*>& lstValues, const unsigned char* pPayload, const int nPayloadSz)
{
	list<ExprValue*>::iterator it;
	for (it = lstValues.begin(); it != lstValues.end(); ++it) {
		if ((*it)->matchValue(pPayload, nPayloadSz)) {
			return true;
		}
	}
	return false;
}
