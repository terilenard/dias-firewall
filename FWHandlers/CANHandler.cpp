#include "CANHandler.h"
#include "../FWCoreLibrary/Config.h"
#include <string>
#include <sys/time.h>
#include <iostream>

#ifndef WIN32
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <cstring>
#include <string>
#endif


CANHandler::CANHandler(const char* pszCfgFile):
    m_sCfgFile(pszCfgFile)
{
#ifndef WIN32
    m_poll.fd = -1;
#endif
    m_callback = NULL;
    m_arg = NULL;
}

CANHandler::~CANHandler()
{
#ifndef WIN32
    if (m_poll.fd != -1) {
        close(m_poll.fd);
    }
#endif
}

bool CANHandler::initialize(void (*callback)(int idx, unsigned char* payload, void* arg, int dlc, long timestamp), void* arg)
{
    m_callback = callback;
    m_arg = arg;

#ifndef WIN32

    const char* fifo;

    fifo = getParameter(m_sCfgFile.c_str(),"canPipe");

    if (!fifo) {
        printf("Error while reading pipe path from config file\n");
        return false;
     }

     printf("Found CAN named pipe: %s\n", fifo);
    // Create the named pipe, read and write rights only for the owner
    int result = mkfifo(fifo, S_IFIFO | S_IRUSR | S_IWUSR);
    if (-1 == result && EEXIST != errno) {
        perror("The creation of the named pipe failed!\n");
        return false;
    }

    printf("Opening pipe...\n");
    m_poll.fd = open(fifo, O_RDONLY | O_NDELAY);
    if (-1 == m_poll.fd) {
        perror("Unable to open the fifo for writing!\n");
        return false;
    }
    printf("Pipe successfully opened.\n");

    m_poll.events = POLLIN;

#endif

    return true;
}

bool CANHandler::runHandler(void)
{
    unsigned long timestamp;
    unsigned int idx;
    unsigned char tidx[4];
    unsigned char dlc[1];
    unsigned char payload[8];
    int sdlc = 0;
  

#ifndef WIN32

    if (-1 == m_poll.fd) {
        return false;
    }

    // Chack if there is something on tht PIPE
    if (poll(&m_poll, 1, 100) < 1) {
        return true;
    }

    // Read CAN ID and payload
    int k = read(m_poll.fd, tidx, 4);
    if (k < 4) {
	printf("Error reading %d CAN ID\n", k);
        return false;
    }

    k =  read(m_poll.fd, dlc, 1);
    if (k < 1) {
	printf("Error reading %d DLC\n",k );
	fflush(stdout);
        return false;
    }

    // Convert the DLC to int
    std::memcpy(&sdlc, dlc, sizeof dlc);

    k =  read(m_poll.fd, payload, sdlc);
    if (k < sdlc) {
	printf("Error reading %d != %d payload\n", k, sdlc);
	fflush(stdout);
        return false;
    }

    idx = (tidx[0] << 24) | (tidx[1] << 16) | (tidx[2] << 8) | tidx[3];

    // Get the timestamp
    struct timeval curTime;
    unsigned long millis = 0;
    unsigned long seconds = 0;

    gettimeofday(&curTime, NULL);
    seconds = (curTime.tv_sec % 100000) * 1000;
    millis = (curTime.tv_usec / 1000);
    timestamp = seconds + millis ;

    // Now fire the callback
    m_callback( (int)idx, payload, m_arg, sdlc, timestamp );

#endif

    return true;
}

