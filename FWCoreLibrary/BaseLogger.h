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

