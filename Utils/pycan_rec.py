"""
This work is licensed under the terms of the MIT license.  
For a copy, see <https://opensource.org/licenses/MIT>.

Developed by NISLAB - Network and Information Security Laboratory
at George Emil Palade University of Medicine, Pharmacy, Science and
Technology of Târgu Mureş <https://nislab.umfst.ro/>

Contributors: Roland Bolboaca
"""
#!/usr/bin/env python3

import can
import time
import struct
import os, sys, traceback
from pycan import *

sys.path.insert(0, '/etc/diasfw/')
from config import *


def listen_and_send(canbus, pipeout, listener_type):
    #cnt=0
    listener = setup_listener(listener_type, LOG_FILE)
    notifier = setup_notifier(listener)

    logger.debug("Listening for CAN messages")

    try:
        a_listener = can.BufferedReader()
        notifier = can.Notifier(canbus, [a_listener])

        while True:
            msg = a_listener.get_message(0.5)
            if msg is not None:
                send_message_on_pipe(pipeout, msg)

    except KeyboardInterrupt:
        os.close(pipeout)
        pass

    except (BrokenPipeError, IOError) as ex:
        logger.error("Exception: " + str(ex) + ", exiting")
        pass

    finally:
         if listener:
             notifier.stop()
         canbus.shutdown()


def main():

    can_bus = setup_bus(CAN_CHANNEL_REC)

    pipeout = create_pipe(PIPE_PATH)

    if can_bus and pipeout:
        listen_and_send(can_bus, pipeout, LISTENER_TYPE)

if __name__ == "__main__":

    try:
        main()

    except KeyboardInterrupt:
        sys.exit(0)
