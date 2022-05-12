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
from .pyc_logger import logger

def setup_listener(type, log_file):
    ### Writing recieved messages to FILE ###
    if type == 0:
        listener = None
    elif type == 1:
        listener = can.Logger(log_file)
    elif type == 2:
        listener = can.ASCWriter(log_file)
    else:
        listener = can.BLFWriter(log_file,'b')

    return listener

def setup_notifier(listener):
    if listener:
        notifier = can.Notifier(bus, [listener, can.Printer()])
    else:
        notifier = None

    return notifier

def read_messages_from_file(log_file):

    logger.debug("Reading from file: ", log_file)
    can_log = can.LogReader(log_file)

    return can_log

def create_message(id, msg_data, extended):
     msg = can.Message(is_extended_id=extended, arbitration_id=id, data = msg_data)
     return msg
