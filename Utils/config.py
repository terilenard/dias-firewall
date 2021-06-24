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

LOG_FILE = "/var/log/dias-firewall/dfw.log"
PIPE_PATH = "/tmp/canfw_pipe"
LISTENER_TYPE = 0

SENDING_DELAY = 0.2
SENDING_TYPE = 0

SENDING_DEMO = 1
SENDING_DELAY_DM1 = 1

CAN_CHANNEL_SEND = 'can0'
CAN_CHANNEL_REC = 'can0'
