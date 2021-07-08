#!/usr/bin/env python3

import can
import time, os
from pycan_msg import *
from pyc_logger import logger

def setup_bus(channel):
    logger.debug("Setting up BUS: " + channel)

    try:
        bus = can.interface.Bus(channel=channel, bustype='socketcan_native')
        return bus

    except Exception as ex:
        logger.error("Error: " + str(ex))
        return None

def create_pipe(pipe_path):
    logger.debug("Opening PIPE: " + pipe_path)

    if os.path.exists(pipe_path):
        os.remove(pipe_path)
        os.mkfifo(pipe_path)
    else:
        os.mkfifo(pipe_path)

    pipeout = os.open(pipe_path, os.O_WRONLY)
    return pipeout

def send_message_on_pipe(pipeout, msg):

    sent_msg = (msg.arbitration_id).to_bytes(4,'big') + (msg.dlc).to_bytes(1,'little') + msg.data

    d = os.write(pipeout, sent_msg)
    ts = time.time()
    print(str(ts) , " Sent : " + str(d) + " bytes. Payload: " + str(sent_msg))

def send_message_on_can(bus, msg):
    bus.send(msg)
