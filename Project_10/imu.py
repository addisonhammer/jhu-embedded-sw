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

    def to_json(self) -> bytes:
        return json.dumps(self.__dict__)

class Arduino:
    def __init__(self, port, baudrate, timeout):
        self.port = port
        self.baud = baudrate
        self.timeout = timeout
        self.serial = None

    def read_data(self) -> ImuData:
        if not self.serial:
          self.serial = serial.Serial(port=self.port, baudrate=self.baud, timeout=self.timeout)
        self.serial.write(_REQ)
        time.sleep(0.05)
        data = self.serial.readline().split(_DELIM)
        # print(f'Data Recieved!: {data}')
        if len(data) == 3:
            args = tuple(float(val) for val in data)
            return ImuData(*args)
        raise SerialException("Error Retrieving IMU Data!")



if __name__ == "__main__":
    a = Arduino(port='/dev/ttyUSB0', baudrate=115200, timeout=.2)
    while True:
        try:
            print(a.read_data().to_json())
        except Exception as e:
            print(e)