#!/usr/bin/env python3

## Listener Types ##
# 0 - Messages won't be printed on screen or written to file
# 1 - Messages will be written to a LOG file
# 2 - Messages will be written to a ASC file
# 3 - Messages will be also written to a BLF file

## Sending Types ##
# 0 - Read from file and send.
# 1 - Send Test Message

## Demo Sending only
# 2 - Send DM1 message (DEMO)

## Sending Delay ##
# Expressed in seconds

LOG_FILE = "/var/log/diasfw/dfw.log"
PIPE_PATH = "/tmp/canfw_pipe"
LISTENER_TYPE = 0

# Not used
#SENDING_DELAY = 0.2
#SENDING_TYPE = 0

# Not used
#SENDING_DEMO = 1
#SENDING_DELAY_DM1 = 1

# The module listens for frames on the REC channel and forwards them to the SEND channel.
#CAN_CHANNEL_SEND = 'vcan0'
CAN_CHANNEL_REC = 'vcan0'

# Internal Logging
LOGFILE = "/var/log/diasfw/pycan.log"
LOGLEVEL = "DEBUG"
