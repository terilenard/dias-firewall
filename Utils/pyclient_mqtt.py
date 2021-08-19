#!/usr/bin/env python3 
import time
import atexit

from argparse import ArgumentParser
from threading import Event
import paho.mqtt.client as mqtt

from pyc_logger import logger

# Dependencies
# pip3 install paho-mqtt

class MQTTClient(object):


    def __init__(self, client_id, password, host, port, cafile):
        self._inst = mqtt.Client()
        self._inst.tls_set(cafile)
        self._inst.username_pw_set(client_id, password)
        self._inst.on_connect = self._on_connect
        self._host = host
        self._port = port
        self._start_event = Event()

    def connect(self):
        self._inst.loop_start()
        self._inst.connect(self._host, self._port, 60)

    def stop(self):
        logger.info("Stopping client...")
        if self._inst.is_connected():
            self._inst.loop_stop()
            self._inst.disconnect()
            logger.info("Client disconnected.")

    def _on_connect(self, client, userdata, flags, rc):

        if rc == 0:
            logger.info("Client connected " + str(rc))
            self._start_event.set()
        else:
            logger.error("Client couldn't connect. Received code: {}.".format(rc))
            logger.info("Client tries reconnect...")
            self._inst.reconnect()

    def push(self):
        logger.info("Wainting for connection to {}:{}.".format(self._host, self._port))
        self._start_event.wait()

       # TODO: read data and publish it
        while True:
            self._inst.publish("telemetry", "{\"test\": 2}")
            logger.info("Published data")
            time.sleep(1)


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

    args = parser.parse_args()

    client = MQTTClient(args.client_id, args.password, args.host, args.port, args.cafile)

    atexit.register(lambda: client.stop)

    try:
        client.connect()
        client.push()
    except KeyboardInterrupt:
        client.stop()
