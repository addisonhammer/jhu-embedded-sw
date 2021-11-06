import socket
import sys
import json
import time
from io import BytesIO
from gpsd_reports import GPSDReport, TPVReport
import signal
import sys

SERVER_IP = '192.168.1.23' # IP of server
SERVER_PORT = 2947 # for gpsd

# per gpsd docs, max size of a response
GPSD_RESP_MAX_SIZE = 10240 
# message to send to enable server to send json reports
WATCH_DEV_STR = '?WATCH={"enable":true,"json":true}' 

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect the socket to the port where the server is listening
server_address = (SERVER_IP, SERVER_PORT)

# attempt to connect to server
try:
    sock.connect(server_address)
    print('Connected to: %s:%s' % server_address)
except:
    print("Failed to connect to: %s:%s" % server_address)
    sys.exit(0)

# function to gracefully exit
def signal_handler(sig, frame):
    sock.close()
    sys.exit(0)

# setup control C handler
signal.signal(signal.SIGINT, signal_handler)
# allow program to do other things while waiting for data
sock.setblocking(False)

# send the watch command to get the server to start sending data
sock.send(WATCH_DEV_STR.encode())

def handle_report(line):
    report = GPSDReport.getReport(json.loads(line))
    if isinstance(report, TPVReport):
        print('Time: %s, Lat: %s, Lon: %s, Alt: %s, Speed: %s' \
            % (report.time, report.latDeg, report.lonDeg, report.altMeters, report.speedMPS))

def no_report():
    # do other things between reports
    time.sleep(0.5)

with BytesIO() as buffer:
    while True:
        try:
            resp = sock.recv(GPSD_RESP_MAX_SIZE)# read in bytes
        except BlockingIOError:
            no_report()
        else:
            buffer.write(resp)          # write to the line buffer
            buffer.seek(0)              # go to beginning of buffer
            start_index = 0             # count number of character processed
            for line in buffer:
                start_index += len(line)
                handle_report(line)     # handle each report

            # handle incomplete line
            if start_index:
                buffer.seek(start_index)
                remaining = buffer.read()
                buffer.truncate(0)
                buffer.seek(0)
                buffer.write(remaining)
            else:
                buffer.seek(0, 2)