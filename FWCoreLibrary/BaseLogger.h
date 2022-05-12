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
#include <string>
using namespace std;

class BaseLogger
{
public:
	BaseLogger();
	virtual ~BaseLogger();

public:
	virtual bool initialize(void) = 0;
	virtual bool logMessage(const string& msg) = 0;
};

