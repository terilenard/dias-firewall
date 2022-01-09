#!/usr/bin/env python3 
import time
import signal
import json

from argparse import ArgumentParser
from threading import Event
import paho.mqtt.client as mqtt

# from pyc_logger import logger

# Dependencies
# pip3 install paho-mqtt

class MQTTClient(object):


    def __init__(self, client_id, password, host, port, cafile, file_name):
        self._inst = mqtt.Client()
        self._inst.tls_set(cafile)
        self._inst.username_pw_set(client_id, password)
        self._inst.on_connect = self._on_connect
        self._host = host
        self._port = port
        self._start_event = Event()
        self._file_name = file_name
        self._file_position = 0

    def connect(self):
        self._load()
        self._inst.loop_start()
        self._inst.connect(self._host, self._port, 60)

    def stop(self):
        # logger.info("Stopping client...")
        if self._inst.is_connected():
            self._inst.loop_stop()
            self._inst.disconnect()
            # logger.info("Client disconnected.")
            self._dump()

    def _on_connect(self, client, userdata, flags, rc):

        if rc == 0:
            # logger.info("Client connected " + str(rc))
            self._start_event.set()
        else:
            # logger.error("Client couldn't connect. Received code: {}.".format(rc))
            # logger.info("Client tries reconnect...")
            self._inst.reconnect()


    def _tail(self, log_file):

        try:
            while self._inst.is_connected():
                line = log_file.readline()

                if not line or not line.endswith('\n'):
                    time.sleep(0.1)
                    continue
                yield line
        except (OSError, KeyboardInterrupt) as ex:
            # logger.error(str(ex))
            self.stop()


    def _load(self):
        try:
            with open("/var/cache/logpublisher/log_cache", "r") as log_cache:
                self._file_position = int(log_cache.readline())
        except FileNotFoundError as ex:
            pass

    def _dump(self):
        try:
            with open("/var/cache/logpublisher/log_cache", "w") as log_cache:
                log_cache.write(str(self._file_position))
        except OSError as ex:
            # logger.error(str(ex))
            return

    def push(self):
        # logger.info("Wainting for connection to {}:{}.".format(self._host, self._port))

        self._start_event.wait()

        log_file = None

        try:
            log_file = open(self._file_name)
            log_file.seek(int(self._file_position))

        except OSError as ex:
            # logger.error(str(ex))
            self._dump()

        while self._inst.is_connected():
            log_lines = self._tail(log_file)
            for line in log_lines:
                self._file_position = log_file.tell()
                data = {"log": line.strip()}
                # print(str(data))
                self._inst.publish("telemetry", json.dumps(data))
            time.sleep(0.1)


def exit_handler(signum, frame):
    global client

    if client:
        client.stop()

client = None

if __name__ == "__main__":

    parser = ArgumentParser(description="MQTT Client which publishes data to Bosch IoT Hub.")
    parser.add_argument("--client_id", type=str, help="Bosch IoT Hub client id under the form <auth_id>@<tenant_id>."
                        "Client needs to be registered via the Bosch IoT Hub - Management Interface. Example: client@t6906174622ffXXXXXXXXX1fefc53459")
    parser.add_argument("--password", type=str, help="Bosch IoT Hub client password. Generated during device creation "
                        "on Bosch IoT Hub - Management Interface.")
    parser.add_argument("--host", type=str, help="Remote MQTT host of Bosch IoT Hub. Usually is mqtt.bosch-iot-hub.com.")
    parser.add_argument("--port", type=int, help="Remote MQTT port of Bosch IoT Hub. Usually is 8883.")
    parser.add_argument("--cafile", type=str, help="Path to the ca file obtained from Bosch IoT Hub. If you don't have it, get it using: "
                        "curl -o iothub.crt https://docs.bosch-iot-suite.com/hub/iothub.crt .")
    parser.add_argument("--log_file", type=str, help="Log file to be monitored. New lines in the log file will be publish via MQTT to Bosch IoT Hub.")
    args = parser.parse_args()

    client = MQTTClient(args.client_id, args.password, args.host, args.port, args.cafile, args.log_file)

    signal.signal(signal.SIGTERM, exit_handler)

    try:
        client.connect()
        client.push()
    except KeyboardInterrupt:
        client.stop()
