#pragma once
#include <map>
#include "BaseLogger.h"
#include <thread>
using namespace std;

class FrequencyProcessor
{
public:
	FrequencyProcessor(const char * fileName, BaseLogger * logobj, const int notifierTimer);
	FrequencyProcessor();
	~FrequencyProcessor();
	int processNewID(const int ID, const long timestamp);
	void frequencyNotifier();
	bool thShouldRun;
	void initNotifier();

private:
	int readFileAndCreateIDFreqMap(const char * fileName);
	int initialize(const char * fileName, BaseLogger * logobj, const int notifierTimer);
	int getIntFromString(const string inputString, int * outputInt);
	int verifyFrequency(const int currFreq, tuple<int, int, int> definedFreq);
	BaseLogger * logobj;

	map<int, tuple<int, int, int> > idFrequencyMap;
	map<unsigned long, unsigned long> idTimestampMap;
	map<int, tuple<int, int, int>>::iterator mapitterator;
	bool thReady = 0;
	thread my_thread_;
	int notifierTimer;
};

