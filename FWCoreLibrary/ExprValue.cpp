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

#include "ExprValue.h"
#include <stdio.h>

ExprValue::ExprValue() :
	m_index(0),
	m_pValue(NULL)
{

}

ExprValue::~ExprValue()
{
	if (NULL != m_pValue) {
		delete m_pValue;
	}

}

void ExprValue::setValue(int index, int value)
{
	if (NULL != m_pValue) {
		delete m_pValue;
	}

	m_index = index;
	m_pValue = dynamic_cast<BasicValue*>(new ExprValue::SingleValue(value));
}

void ExprValue::setValue(int index, int lowerBound, int upperBound)
{
	if (NULL != m_pValue) {
		delete m_pValue;
	}

	m_index = index;
	m_pValue = dynamic_cast<BasicValue*>(new ExprValue::BoundedValue(lowerBound, upperBound));
}

void ExprValue::printValue(void)
{
	printf("index='%d', ", m_index);
	m_pValue->printValue();
}

bool ExprValue::matchValue(const unsigned char* pPayload, const int nPayloadSz)
{
	if (m_index >= nPayloadSz) {
		return false;
	}
	return m_pValue->matchValue(pPayload, m_index);
}