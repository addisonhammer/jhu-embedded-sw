import json
import dataclasses
import serial
import time

from serial.serialutil import SerialException

_REQ = b'!'
_DELIM = b','


@dataclasses.dataclass(frozen=True, order=True)
class ImuData:
    yaw: float
    roll: float
    pitch: float

    def to_json(self) -> str:
        return json.dumps(self.__dict__)


class Arduino:
    def __init__(self, port, baudrate, timeout):
        self.port = port
        self.baud = baudrate
        self.timeout = timeout
        self.serial = None

    def read_data(self) -> ImuData:
        if not self.serial:
            self.serial = serial.Serial(
                port=self.port, baudrate=self.baud, timeout=self.timeout)
        self.serial.write(_REQ)
        time.sleep(0.05)
        data = self.serial.readline().split(_DELIM)
        # print(f'Data Recieved!: {data}')
        if len(data) == 3:
            args = tuple(float(val) for val in data)
            return ImuData(*args)
        raise SerialException("Error Retrieving IMU Data!")

    def read_json_bytes(self) -> bytes:
        return self.read_data().to_json().encode('utf-8')



def try_connect():
    try:
        return serial.Serial(port='/dev/rfcomm0', baudrate=115200, timeout=0.2)
    except serial.serialutil.SerialException as e:
        print('Bluetooth client not connected:')
        print(e)
        print('Retrying in 5s...')
        time.sleep(5)
        return None


if __name__ == "__main__":
    a = Arduino(port='/dev/ttyUSB0', baudrate=115200, timeout=0.2)
    while True:
        if not client:
            client = try_connect()
        try:
            client.write(a.read_json_bytes())
        except Exception as e:
            print(e)
            time.sleep(1)
            client = None
            continue
