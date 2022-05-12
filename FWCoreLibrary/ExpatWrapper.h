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
#include "expat.h"
#include <string>
using namespace std;

class ExpatWrapper
{
public:
	ExpatWrapper();
	virtual ~ExpatWrapper();

	bool initialise(void);
	void destroy(void);

public:
	bool parseFile(const char* pFileName);

private:
	void enableHandlers(void);

	// Data handlers
	static void startElementHandler(void* pUserData, const XML_Char* pszName, const XML_Char** pAttrs);
	static void endElementHandler(void* pUserData, const XML_Char* pName);
	static void valueHandler(void* pUserData, const XML_Char* pValue, int nLen);

protected:

	// Callback functions
	virtual void onStartElement(const XML_Char* pName, const XML_Char** pAttrs);
	virtual void onEndElement(const XML_Char* pName);
	virtual void onValue(const XML_Char* pName, const int nLen);

	// Semantic (i.e. implementation specific) error handling
	virtual bool isSemanticElementValid(void);

	// Post destroy function
	virtual void onPostDestroy(void);

	bool readAttrValue(const XML_Char** pAttrs, const string& attrName, string& value);

private:

	XML_Parser	m_pParser;

};

