from json.decoder import JSONDecodeError
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

def connectToHost(server_address):
    # Create a TCP/IP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        # attempt to connect to server
        sock.connect(server_address)            
    except:
        return None

    # allow program to do other things while waiting for data
    sock.setblocking(False)
    return sock

# Connect the socket to the port where the server is listening
serverAddress = (SERVER_IP, SERVER_PORT)
sock = connectToHost(serverAddress)
if sock == None:
    print("Failed to connect to: %s:%s" % serverAddress)
    sys.exit(0)
else:
    print('Connected to: %s:%s' % serverAddress)

# function to gracefully exit
def signalHandler(sig, frame):
    print('Disconnecting from: %s:%s' % serverAddress)
    sock.close()
    sys.exit(0)

# setup control C handler
signal.signal(signal.SIGINT, signalHandler)

# send the watch command to get the server to start sending data
sock.send(WATCH_DEV_STR.encode())

def handleReport(line):
    try:
        report = GPSDReport.getReport(json.loads(line))
    except JSONDecodeError:
        print("JSON Decode ERR:" + line)
        return

    if isinstance(report, TPVReport):
        # only print reports if we have a fix
        if report.mode >= TPVReport.ModeEnum.DIM_2.value:
            print('Time: %s, Lat: %s, Lon: %s, Alt: %s, Heading: %s, Speed: %s, Climb: %s' \
                % (report.time, report.latDeg, report.lonDeg, report.altMeters, \
                    report.headingDeg, report.speedMPS, report.climbMPS))
    elif report == None:
        print("Unknown Report Type")

def noReport():
    # do other things between reports
    time.sleep(0.1)

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
            noReport()
        except ConnectionError:
            print('Connection Closed: %s:%s' % serverAddress)
            break
        else:
            if not resp:
                # Error connection closed
                print('Connection Closed: %s:%s' % serverAddress)
                break

            buffer.write(resp)          # write to the line buffer
            buffer.seek(0)              # go to beginning of buffer
            start_index = 0             # count number of character processed
            for line in buffer:
                start_index += len(line)
                handleReport(line)     # handle each report

            # handle incomplete line
            if start_index:
                buffer.seek(start_index)
                remaining = buffer.read()
                buffer.truncate(0)
                buffer.seek(0)
                buffer.write(remaining)
            else:
                buffer.seek(0, 2)

        
