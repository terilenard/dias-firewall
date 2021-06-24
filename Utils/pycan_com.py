#!/usr/bin/env python3

import can
import time, os
from pycan_msg import *

def setup_bus(can_channel):
    print("Setting up BUS")

    try:
        bus = can.interface.Bus(channel=can_channel, bustype='socketcan_native')

        return bus

    except Exception as ex:
        print("Error: " + str(ex))

        return None


def create_pipe(pipe_path):
    print("Opening PIPE")

    if os.path.exists(pipe_path):
        os.remove(pipe_path)
        os.mkfifo(pipe_path)
    else:
        os.mkfifo(pipe_path)

    pipeout = os.open(pipe_path, os.O_WRONLY)

    return pipeout

def send_message_on_pipe(pipeout, msg):

    sent_msg = (msg.arbitration_id).to_bytes(4,'big') + msg.data

    d = os.write(pipeout, sent_msg)
    ts = time.time()
    print(str(ts) , " Sent : " + str(d) + " bytes. Message: " + str(sent_msg))

def send_message_rep(bus, delay, id, msg_data):
    msg = create_message(id, msg_data, False)

    while 1:
        send_one_message(bus,msg)
        time.sleep(delay)


def send_file_messages_can(bus, message_list, delay):

     if (bus):
         print("Sending Messages")

         for msg in message_list:
             print(msg)

             send_one_message_can(bus, msg)
             time.sleep(delay)

def send_one_message_can(bus, msg):
    bus.send(msg)
