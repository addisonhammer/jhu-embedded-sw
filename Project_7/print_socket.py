import socket
import sys
import json
import time
from gpsd_reports import GPSDReport, TPVReport
import signal
import sys

SERVER_IP = '10.3.141.1'  # IP of server
SERVER_PORT = 2947  # for gpsd

# per gpsd docs, max size of a response
GPSD_RESP_MAX_SIZE = 10240
# message to send to enable server to send json reports
WATCH_DEV_STR = '?WATCH={"enable":true,"json":true}'

def connect_socket(ip: str = SERVER_IP,
                   port: int = SERVER_PORT) -> socket.socket:
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_address = (ip, port)
        sock.connect(server_address)
        print(f'Connected to: {ip}:{port}')
        return sock
    except:
        print(f'Failed to connect to: {ip}:{port}')
        sys.exit(0)


def setup_sentinel(sock):
    # function to gracefully exit
    def signal_handler(sig, frame):
        sock.close()
        sys.exit(0)

    # setup control C handler
    signal.signal(signal.SIGINT, signal_handler)


def handle_report(message: str):
    report = GPSDReport.getReport(json.loads(message))
    if isinstance(report, TPVReport):
        print(f'Time: {report.time}, ')
        print(f'Lat: {report.latDeg}, ')
        print(f'Lon: {report.lonDeg}, ')
        print(f'Alt: {report.altMeters}, ')
        print(f'Speed: {report.speedMPS}')
        print('-----------------------')


def watch_socket(sock: socket.socket):
    # allow program to do other things while waiting for data
    sock.setblocking(False)
    # send the watch command to get the server to start sending data
    sock.send(WATCH_DEV_STR.encode())

    while True:
        partial_message = b''
        try:
            bytes_recv = sock.recv(GPSD_RESP_MAX_SIZE)  # read in bytes
            messages = bytes_recv.split(b'\r\n')
            messages[0] = partial_message + messages[0] # prepend the partial message
            partial_message = messages.pop()  # This should be blank, save for next loop
            for message in messages:
                handle_report(message)
        except BlockingIOError:
            time.sleep(0.1)
        except Exception as e:
            print(e)


if __name__ == '__main__':
    sock = connect_socket()
    setup_sentinel(sock)
    watch_socket(sock)
