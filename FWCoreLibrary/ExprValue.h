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
#include <stdio.h>

class ExprValue
{
private:
	class BasicValue {
		public:
			BasicValue() {}
			virtual ~BasicValue() {}

			virtual void printValue(void) = 0;
			virtual bool matchValue(const unsigned char* pPayload, int index) = 0;
	};
	class SingleValue : public BasicValue {
	public:
		SingleValue(int value) : m_value(value) {}
		virtual ~SingleValue() {}
		int m_value;

		void printValue(void) {
			printf("value='%d'", m_value);
		}

		bool matchValue(const unsigned char* pPayload, int index) {
			return pPayload[index] == (unsigned char)m_value;
		}
	};
	class BoundedValue : public BasicValue {
	public:
		BoundedValue(int lowerBound, int upperBound) : m_lowerBound(lowerBound), m_upperBound(upperBound) {}
		virtual ~BoundedValue() {}
		int m_lowerBound;
		int m_upperBound;

		void printValue(void) {
			printf("lowerBound='%d', upperBound='%d'", m_lowerBound, m_upperBound);
		}

		bool matchValue(const unsigned char* pPayload, int index) {
			return ((pPayload[index] >= (unsigned char)m_lowerBound) && (pPayload[index] <= (unsigned char)m_upperBound));
		}
	};

public:
	ExprValue();
	virtual ~ExprValue();

	void setValue(int index, int value);
	void setValue(int index, int lowerBound, int upperBound);

	void printValue(void);

	bool matchValue(const unsigned char* pPayload, const int nPayloadSz);

private:
	int m_index;
	BasicValue* m_pValue;
};

