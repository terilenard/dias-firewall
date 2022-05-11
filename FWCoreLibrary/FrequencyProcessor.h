/*
* This work is licensed under the terms of the MIT license.  
* For a copy, see <https://opensource.org/licenses/MIT>.
*
* Developed by NISLAB - Network and Information Security Laboratory
* at George Emil Palade University of Medicine, Pharmacy, Science and
* Technology of Târgu Mureş <https://nislab.umfst.ro/>
*
* Contributors: Roland Bolboaca, Teri Lenard
*/

#pragma once
#include <map>
#include "BaseLogger.h"
#include <thread>
#include <mutex>


using namespace std;

class FrequencyProcessor
{
public:
	FrequencyProcessor(const char * fileName, BaseLogger * logobj, const int notifierTimer);
	FrequencyProcessor();
	~FrequencyProcessor();
	int processNewID(const int ID, const unsigned long long timestamp);
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
	map<int, unsigned long long> idTimestampMap;
	map<int, tuple<int, int, int>>::iterator mapitterator;
	bool thReady = 0;
	thread my_thread_;
	int notifierTimer;
	mutex mtx,mtx2;
};

