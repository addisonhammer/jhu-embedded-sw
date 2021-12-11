import sys

from gps3 import gps3
import imu
from flask import Flask, render_template
from turbo_flask import Turbo
import threading

app = Flask(__name__)
turbo = Turbo(app)
arduino = None
gps_socket = None
gps_data = gps3.DataStream()


@app.before_first_request
def before_first_request():
    threading.Thread(target=update_load).start()
    threading.Thread(target=update_gps).start()

def update_gps():
    global gps_socket
    global gps_data
    while True:
        try:
            if not gps_socket:
                gps_socket = gps3.GPSDSocket()
                gps_socket.connect()
                gps_socket.watch()
            for new_data in gps_socket:
                if new_data:
                    gps_data = gps3.DataStream()
                    gps_data.unpack(new_data)
                    print(gps_data.TPV)
        except Exception as e:
            print(e)
        finally:
            gps_socket.close()
            gps_socket = None


def update_load():
    with app.app_context():
        while True:
            turbo.push(turbo.replace(render_template('imu.html.jinja'), 'imu'))


@app.context_processor
def inject_load():
    global arduino
    global gps_data
    data_dict = {
        'yaw': 0, 'pitch': 0, 'roll': 0,
        'lat': 0, 'lon': 0, 'alt': 0,
    }
    try:
        data_dict.update(gps_data.TPV)
        imu_data = arduino.read_data().__dict__
        data_dict.update(imu_data)
    except Exception as e:
        print(e)
    # print(data_dict)
    return data_dict


@app.route('/')
def index():
    return render_template('index.html.jinja')


@app.route("/imu")
def get_imu():
    global arduino
    return arduino.read_data().to_json()

if __name__ == "__main__":
    if len(sys.argv) < 2:
        port = '/dev/ttyUSB0'
    else:
        port = sys.argv[1]
    arduino = imu.Arduino(port=port, baudrate=115200, timeout=0.1)
    gps_socket = gps3.GPSDSocket()
    gps_socket.connect()
    gps_socket.watch()
    app.run(host="0.0.0.0", port=5000, debug=True)
