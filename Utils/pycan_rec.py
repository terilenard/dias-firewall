#!/usr/bin/env python3

import can
import time
import struct
import os, sys, traceback
from pycan import *

sys.path.insert(0, '/etc/diasfw/')
from config import *

def listen_and_send(canbus, pipeout, listener_type):

    listener = setup_listener(listener_type, LOG_FILE)
    notifier = setup_notifier(listener)

    logger.debug("Listening for CAN messages")

    try:
        while True:
            msg = canbus.recv(1)

            if msg:
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
