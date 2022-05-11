/*
* This work is licensed under the terms of the MIT license.  
* For a copy, see <https://opensource.org/licenses/MIT>.
*
* Developed by NISLAB - Network and Information Security Laboratory
* at George Emil Palade University of Medicine, Pharmacy, Science and
* Technology of Târgu Mureş <https://nislab.umfst.ro/>
*
* Contributors: Bela Genge, Roland Bolboaca
*/

#include "../FWCoreLibrary/FWCore.h"
#include "CANHandler.h"
#include <stdio.h>
#include<unistd.h>

#ifndef WIN32
#include <signal.h>
#endif

bool g_exit = false;

void sig_handler(int sig)
{
    printf("Recieved signal %d\n", sig);
    g_exit = true;
}

void CAN_callback(int idx, unsigned char* payload, void* arg, int dlc, unsigned long long timestamp);

int main()
{
#ifndef WIN32
    signal(SIGINT, sig_handler);
#endif


    //  Create the FW instance
    int fwInst = createFWInstance("/etc/diasfw/diasfw.cfg");
    if (fwInst == FWCORE_ERROR) {
         printf("Error while creating new fw instance");
         return -1;
    }

    printf("Init can handler...\n");

    CANHandler hCAN("/etc/diasfw/diasfw.cfg");
    if (hCAN.initialize(CAN_callback, (void*)&fwInst) == false) {
        printf("Unable to initialize CAN handler!\n");
        return -1;
    }

    printf("Pres CTRL+C to exit Realt-time CAN\n");

    fflush(stdout);
    
    // Start the Freq Thread
    
    // Main loop
    while(!g_exit)
    {
	 hCAN.runHandler();
	 //usleep(10000);
    }

    printf("Real time CAN exiting ...\n");

	return 0;
}

void CAN_callback(int idx, unsigned char* payload, void* arg, int dlc, unsigned long long timestamp)
{
    int fwInst = *(int*)arg;

    //printf("Received CAN frame with idx: %d, payload: ", idx);
    //for (int i = 0 ; i < dlc ; ++i) {
    //    printf("%X ", payload[i]);
    //}
    //printf("\n");

    processMessage(fwInst, idx, payload, dlc, timestamp);
    //fflush(stdout);
}
