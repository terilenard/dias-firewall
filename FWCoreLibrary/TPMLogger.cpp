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

#include "TPMLogger.h"
#include "Config.h"

#ifndef WIN32
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#endif

TPMLogger::TPMLogger(const char* pszCfgFile):
    m_fd(-1), m_sCfgFile(pszCfgFile)
{

}

TPMLogger::~TPMLogger()
{
#ifndef WIN32
    if (m_fd != -1) {
        close(m_fd);
    }
#endif
}

bool TPMLogger::initialize(void) {

#ifndef WIN32

    const char* fifo;

    fifo = getParameter(m_sCfgFile.c_str(),"tpmPipe");

    if (!fifo) {
        printf("Error while reading pipe path from config file\n");
        return false;
     }

    printf("Found TPM named pipe: %s\n", fifo);

    // Create the named pipe, read and write rights only for the owner
    int result = mkfifo(fifo, S_IFIFO | S_IRUSR | S_IWUSR);
    if (-1 == result && EEXIST != errno) {
        perror("The creation of the named pipe failed!\n");
        return false;
    }

    m_fd = open(fifo, O_WRONLY);
    if (-1 == m_fd) {
        perror("Unable to open the fifo for writing!\n");
        return false;
    }

#endif

    return true;
}

bool TPMLogger::logMessage(const string& msg)
{
#ifndef WIN32
    if (-1 == m_fd) {
        return false;
    }

    if (write(m_fd, msg.c_str(), msg.size() + 1) < 0) {
        perror("Unable to write log message!");
        return false;
    }

    //printf("Message '%s' was sent to TPM for logging!\n", msg.c_str());
#endif

    return true;
}
