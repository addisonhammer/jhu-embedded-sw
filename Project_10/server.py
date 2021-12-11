import sys
import time

from gps3 import gps3
import imu
from flask import Flask, render_template
from turbo_flask import Turbo
import threading

app = Flask(__name__)
turbo = Turbo(app)
gps_socket = None
gps_data = gps3.DataStream()
imu_data = {}


@app.before_first_request
def before_first_request():
    threading.Thread(target=update_load).start()
    threading.Thread(target=update_gps).start()
    threading.Thread(target=update_imu).start()

def update_gps():
    global gps_socket
    global gps_data
    while True:
        try:
            if not gps_socket:
                gps_socket = gps3.GPSDSocket()
                gps_socket.connect()
                gps_socket.watch()
                print('connected')
            for new_data in gps_socket:
                if new_data:
                    temp = gps3.DataStream()
                    temp.unpack(new_data)
                    if temp.TPV['lat'] != 'n/a':
                        gps_data = temp.TPV
                    print(temp.TPV)
        except Exception as e:
            print(f'update_gps error: {e}')
        finally:
            gps_socket.close()
            gps_socket = None

def update_imu():
    global imu_data
    while True:
        try:
            imu_data = imu.read_data()
        except Exception as e:
            print(f'update_imu error: {e}')
        finally:
            time.sleep(1)
            # print(f'Imu Data: {imu_data}')

def update_load():
    with app.app_context():
        while True:
            turbo.push(turbo.replace(render_template('imu.html.jinja'), 'imu'))
            turbo.push(turbo.replace(render_template('gps.html.jinja'), 'gps'))

data_dict = {}
@app.context_processor
def inject_load():
    global gps_data
    global imu_data
    global data_dict
    try:
        data_dict.update(gps_data)
        if imu_data:
            data_dict.update(imu_data)
    except Exception as e:
        print(f'inject_load error: {e}')
    # print(data_dict)
    return data_dict


@app.route('/')
def index():
    return render_template('index.html.jinja')


@app.route("/imu")
def get_imu():
    return imu.read_data()

if __name__ == "__main__":
    if len(sys.argv) < 2:
        port = '/dev/ttyUSB0'
    else:
        port = sys.argv[1]
    gps_socket = gps3.GPSDSocket()
    gps_socket.connect()
    gps_socket.watch()
    app.run(host="0.0.0.0", port=5000, debug=True)
