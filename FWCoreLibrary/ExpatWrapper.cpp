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

#define _CRT_SECURE_NO_DEPRECATE
#include "ExpatWrapper.h"
#include <stdio.h>
#include <cstring>
using namespace std;

ExpatWrapper::ExpatWrapper() :
	m_pParser(NULL)
{

}

ExpatWrapper::~ExpatWrapper()
{
	if (NULL != m_pParser) {
		XML_ParserFree(m_pParser);
		m_pParser = NULL;
	}
}

bool ExpatWrapper::initialise(void)
{
	// Destroy old parser
	if (NULL != m_pParser) {
		destroy();
	}

	// Create a new parser
	m_pParser = XML_ParserCreate_MM(NULL, NULL, NULL);
	if (NULL == m_pParser) {
		return false;
	}

	// Set user data used in callbacks
	XML_SetUserData(m_pParser, (void*)this);

	// Enable handlers
	enableHandlers();

	return true;
}

void ExpatWrapper::destroy(void)
{
	if (NULL != m_pParser) {
		XML_ParserFree(m_pParser);
		m_pParser = NULL;
	}
}

bool ExpatWrapper::parseFile(const char* pFileName)
{
	if (NULL == m_pParser) {
		return false;
	}

	// Open file, read all and parse
	FILE* pf = fopen(pFileName, "rt");
	if (NULL == pf) {
		printf("ExpatWrapper: Open File Error\n");
		return false;
	}

	string sFullFile = "";
	char pBuff[1024];
	while (!feof(pf)) {
		if (NULL != fgets(pBuff, sizeof(pBuff) - 1, pf)) {
			pBuff[sizeof(pBuff) - 1] = '\0';
			sFullFile += pBuff;
		}
	}

	if (0 == XML_Parse(m_pParser, sFullFile.c_str(), sFullFile.length(), true)) {
		printf("XML parsing Error");
		return false;
	}

	return true;
}

void ExpatWrapper::enableHandlers(void)
{
	// Enable start element handler
	XML_SetStartElementHandler(m_pParser, startElementHandler);

	// Enable end element handler
	XML_SetEndElementHandler(m_pParser, endElementHandler);

	// Enable the characterdata handler
	XML_SetCharacterDataHandler(m_pParser, valueHandler);
}

void ExpatWrapper::startElementHandler(void* pUserData, const XML_Char* pName, const XML_Char** pAttrs)
{
	ExpatWrapper* pWrapper = static_cast<ExpatWrapper*>(pUserData);
	pWrapper->onStartElement(pName, pAttrs);

	if (!pWrapper->isSemanticElementValid()) {
		XML_StopParser(pWrapper->m_pParser, false);
	}
}

void ExpatWrapper::endElementHandler(void* pUserData, const XML_Char* pName)
{
	ExpatWrapper* pWrapper = static_cast<ExpatWrapper*>(pUserData);
	pWrapper->onEndElement(pName);

	if (!pWrapper->isSemanticElementValid()) {
		XML_StopParser(pWrapper->m_pParser, false);
	}
}

void ExpatWrapper::valueHandler(void* pUserData, const XML_Char* pValue, int nLen)
{
	ExpatWrapper* pWrapper = static_cast<ExpatWrapper*>(pUserData);
	pWrapper->onValue(pValue, nLen);
}

void ExpatWrapper::onStartElement(const XML_Char* pName, const XML_Char** pAttrs)
{
	while (*pAttrs != NULL)
	{
		printf("\t\t\tText: %s\n", *pAttrs);
		++pAttrs;
	}
}

void ExpatWrapper::onEndElement(const XML_Char* pName)
{
}

bool ExpatWrapper::isSemanticElementValid(void)
{
	return true;
}

void ExpatWrapper::onPostDestroy(void)
{
}

void ExpatWrapper::onValue(const XML_Char* pName, int nLen)
{
	printf("=== VALUE ==-=\n");
}

bool ExpatWrapper::readAttrValue(const XML_Char** pAttrs, const string& attrName, string& value)
{
	if (NULL == pAttrs) {
		return false;
	}

	while (*pAttrs) {
		if (!strcmp(*pAttrs, attrName.c_str())) {
			++pAttrs;
			if (NULL == *pAttrs) {
				return false;
			}
			value = *pAttrs;
			return true;
		}
		++pAttrs;
	}
	return false;
}
