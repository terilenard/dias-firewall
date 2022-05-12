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

#include "FrequencyProcessor.h"
#include "FWCore.h"
#include <map>
#include <ctime>
#include <string>
#include <fstream>
#include <iostream>
#include <mutex> 
#include <sstream>
#include <vector>
#include "BaseLogger.h"

#ifdef _WIN32
#include <chrono>
#include <windows.h>
#endif

#ifdef __linux__ 
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include "TPMLogger.h"
#include <thread>
#endif

using namespace std;

//mutex mtx,mtx2;

FrequencyProcessor::FrequencyProcessor()
{
	
}

FrequencyProcessor::FrequencyProcessor(const char * fileName, BaseLogger * logobj, const int notifierTimer)
{
	if (initialize(fileName, logobj, notifierTimer) !=0)
		{
			cout<<" Initialize Error" <<endl;
		}

	cout<<"--- Frequency Processor Loaded Succesfully ---" <<endl;
}


FrequencyProcessor::~FrequencyProcessor()
{
	thShouldRun = false;
	my_thread_.join();
}

int FrequencyProcessor::processNewID(const int ID, const unsigned long long timestamp)
{
	/* Process a new ID:
					 - Convert the ID and timestamp to int
					 - Check if the ID is defined in the ID Frequency data structure and compute the new frequency based on the previous timestamp
					 - If the ID exists in the ID Freqnecy structure but has not yet been seen, save the current timestamp
						*/

	int currFreq;
	tuple<int, int, int> definedFreq;

	mapitterator = idFrequencyMap.find(ID);

	if (mapitterator != idFrequencyMap.end())
		{
			if (idTimestampMap.find(ID)->second == 0)
			{
				// This only happens the first time a new ID is encountered
				mtx.lock();
				idTimestampMap.at(ID) = timestamp;
				mtx.unlock();

				thReady = 1;
				return 0;
			}

			// Compute the current time diff
			//////mtx.lock();
			currFreq = timestamp - idTimestampMap.find(ID)->second;

			// Update the timestamp for this ID
			idTimestampMap.at(ID) = timestamp;
			//////mtx.unlock();

			definedFreq = idFrequencyMap.at(ID);
			//std::cout<<"@@@@@@@@ " <<  ID << " " << currFreq << std::endl;

			//std::cout<<"@@@@@@@@@ "<< timestamp << " - " << idTimestampMap.find(ID)->second << std::endl;
			return verifyFrequency(currFreq, definedFreq);
		}
		return 0;

}

int FrequencyProcessor::initialize(const char * fileName, BaseLogger * logobj, const int notifierTimer)
{
	/*  Read the IDs and Frequencies from the frequency file.
		Create the ID_FREQ map.
	*/
	this->logobj = logobj;
	this->notifierTimer = notifierTimer;

	return readFileAndCreateIDFreqMap(fileName);
}

int FrequencyProcessor::verifyFrequency(const int currFreq, tuple<int, int, int> definedFreq)
{
	if (currFreq < 0)
	{
		return FWCORE_FREQ_ERROR;
	}

	int lowThreshold, actualFreq, highThreshold;
	tie(lowThreshold, actualFreq, highThreshold) = definedFreq;

	if (lowThreshold <= currFreq && currFreq <= highThreshold)
	{
		return FWCORE_FREQ_OK;
	}
	//else if (actualFreq < currFreq && currFreq <= highThreshold)
	//{
	//	return FWCORE_FREQ_OK;
	//}
	std::cout<<lowThreshold<< "-" << currFreq << " - " << highThreshold<<std::endl;
	return FWCORE_FREQ_BAD;
}


int FrequencyProcessor::readFileAndCreateIDFreqMap(const char * fileName)
{
	string idStr, freqStr[3];
	ifstream freqFile(fileName);
	int idInt, freqInt[3];

	if (freqFile.is_open()) {

		while (freqFile.good()) {

			freqFile >> idStr;
			if (getIntFromString(idStr, &idInt) != 0)
			{
				std::cout << "Error reading ID from frequency file" << std::endl;

				goto error;
			}

			for (int i = 0; i < 3; ++i)
			{
				freqFile >> freqStr[i];
				if ((getIntFromString(freqStr[i], &freqInt[i]) != 0) || freqInt[i] <=0)
				{
					std::cout << "Error reading Frequencies from frequency file" << std::endl;

					goto error;
				}
			}

			for (int i = 0; i < 2; ++i)
			{
				if (freqInt[i] >freqInt[i+1])
				{
					std::cout << "Error in Frequency values" << std::endl;

					goto error;
				}
			}

			idFrequencyMap.insert(make_pair(idInt, make_tuple(freqInt[0], freqInt[1], freqInt[2])));

			mtx.lock();
			idTimestampMap.insert(pair<int, unsigned long long>(idInt, 0));
			mtx.unlock();

		}
	}
	else
	{
		std::cout << "Error opening frequency file" << std::endl;
		return 1;
	}

	freqFile.close();
	cout << "Frequency file loaded!" << endl;
	return 0;

error:
	freqFile.close();
	return 1;
}


void FrequencyProcessor::frequencyNotifier()
{
	struct timeval current_time;
	unsigned long long duration = 0, delay = 0;
	map <int, unsigned long long>::iterator it;
	double duration_d = 0.0;
	stringstream message;
	vector<string> alerts;

	while (thShouldRun)
	{
		
		if (thReady == 1)
		{
			// Get current time
			unsigned int  millis = 0;
			unsigned long long seconds = 0;
			gettimeofday(&current_time, NULL);

			seconds = ((unsigned long long)current_time.tv_sec) * 1000;
			millis = (current_time.tv_usec / 1000);
			duration = seconds + millis ;

			duration_d = (seconds + millis) / 1000.00;
			//cout << duration << " " << duration/1000.0 << " " << duration_d<<endl;

			mtx.lock();
			it = idTimestampMap.begin();

			while (it != idTimestampMap.end())
			{
				delay = duration - it->second;
				if (((duration - it->second) > (notifierTimer * 1000)) && (it->second != 0))
				{
					//message.clear();
					//message.str(string());

					//message << "Frame not recieved for: " << delay << "ms CAN ID: " << it->first <<" . Timestamp: " << to_string(duration_d) << " . \n";
					
					alerts.push_back("Frame not recieved for: " + to_string(delay) + "ms CAN ID: " + to_string(it->first) + " . Timestamp: " + to_string(duration_d) +  " . \n");
					//logobj->logMessage("Frame not recieved for: " + to_string(delay) + "ms CAN ID: " + to_string(it->first) + " . Timestamp: " + to_string(duration_d) +  " . \n");
					it ->second = 0;
					thReady = 0;
				}
				it++;
			}
			mtx.unlock();

			if ((alerts.size() != 0) && (alerts.size() != idTimestampMap.size()))
			{
				mtx.lock();
				for (std::vector<string>::iterator it = alerts.begin(); it != alerts.end(); ++it)
				{
				    logobj->logMessage(*it);
				}
				    //std::cout << *it << " ";
				mtx.unlock();
			}

			alerts.clear();

			if (message.rdbuf()->in_avail() != 0)
			{
				if (logobj == NULL)
				{
					cout<<"Secure Logging: ";
					std::cout<<message.str()<<std::endl;
				}
			}
		}
		sleep(notifierTimer);
		//message.clear();
		//message.str(string());
	}
}

int FrequencyProcessor::getIntFromString(const string inputString, int* outputInt)
{
	try
	{
		*outputInt = stoi(inputString);
	}
	catch (invalid_argument& e) {
		cout << "Error while reading frequency file: invalid value" << std::endl;
		return 1;
	}
	catch (out_of_range& e) {
		cout << "Error while reading frequency file: value out of range" << std::endl;
		return 1;
	}
	catch (...) {
		cout << "Unkown Error while reading from frequency file" << std::endl;
		return 1;
	}

	return 0;
}

void FrequencyProcessor::initNotifier()
{
	my_thread_ = thread(&FrequencyProcessor::frequencyNotifier, this);
}






