from json.decoder import JSONDecodeError
import socket
import sys
import json
import time
from typing import Optional
from gpsd_reports import GPSDReport, TPVReport
import signal
import sys

SERVER_IP = '10.3.141.1'  # IP of server
SERVER_PORT = '2947'  # for gpsd

# per gpsd docs, max size of a response
GPSD_RESP_MAX_SIZE = 10240
# message to send to enable server to send json reports
WATCH_DEV_STR = '?WATCH={"enable":true,"json":true}'


def connectToHost(ip: str = SERVER_IP, port: str = SERVER_PORT) -> Optional[socket.socket]:
    # Create a TCP/IP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        # attempt to connect to server
        server_address = (ip, int(port))
        sock.connect(server_address)
        print(f'Connected to: {ip}:{port}')
    except:
        print(f'Failed to connect to: {ip}:{port}')
        return None

    # allow program to do other things while waiting for data
    sock.setblocking(False)
    return sock


def setup_sentinel(sock: socket.socket):
    # function to gracefully exit
    def signalHandler(sig, frame):
        print('Disconnecting!')
        sock.close()
        sys.exit(0)

    # setup control C handler
    signal.signal(signal.SIGINT, signalHandler)


def handleReport(line):
    try:
        report = GPSDReport.getReport(json.loads(line))
    except JSONDecodeError:
        print(f'JSON Decode ERR: {line}')
        return

    if isinstance(report, TPVReport):
        # only print reports if we have a fix
        if report.mode in (TPVReport.ModeEnum.DIM_2, TPVReport.ModeEnum.DIM_3):
            print(f'Time: {report.time}, '
            f'Lat: {report.latDeg}, '
            f'Lon: {report.lonDeg}, '
            f'Alt: {report.altMeters}, '
            f'Heading: {report.headingDeg}, '
            f'Speed: {report.speedMPS}, '
            f'Climb: {report.climbMPS}')
    elif report == None:
        print('Unknown Report Type!')
        print(f'Raw data: {line}')


def watch_socket(sock: socket.socket):
    sock.send(WATCH_DEV_STR.encode())
    while True:
        partial_message = b''
        try:
            bytes_recv = sock.recv(GPSD_RESP_MAX_SIZE)  # read in bytes
            messages = bytes_recv.split(b'\r\n')
            # prepend the partial message
            messages[0] = partial_message + messages[0]
            partial_message = messages.pop()  # This should be blank, save for next loop
            for message in messages:
                handleReport(message)
        except BlockingIOError:
            time.sleep(0.1)
        except ConnectionError:
            print('Connection Closed!')
            break
        except Exception as e:
            print(e)


def main(*args):
    if len(args) not in (0, 2):
        print('usage: print_socket.py [<ip> <port>]')
        print(f'recieved: {args}')
        return
    # Connect the socket to the port where the server is listening
    sock = connectToHost(*args)
    if not sock:
        return
    setup_sentinel(sock)
    # send the watch command to get the server to start sending data
    watch_socket(sock)


if __name__ == "__main__":
   main(*sys.argv[1:])
