import sys
import imu
from flask import Flask, render_template
from turbo_flask import Turbo
import threading

app = Flask(__name__)
turbo = Turbo(app)
arduino = None


@app.before_first_request
def before_first_request():
    threading.Thread(target=update_load).start()


def update_load():
    with app.app_context():
        while True:
            turbo.push(turbo.replace(render_template('imu.html.jinja'), 'imu'))


@app.context_processor
def inject_load():
    global arduino
    try:
        data = arduino.read_data()
    except:
        return {'yaw': 0, 'pitch': 0, 'roll': 0}
    return data.__dict__


@app.route('/')
def index():
    return render_template('index.html.jinja')


@app.route("/imu")
def get_imu():
    global arduino
    return arduino.read_data().to_json()

if __name__ == "__main__":
    if len(sys.argv) < 2:
        port = '/dev/arduino_0'
    else:
        port = sys.argv[1]
    arduino = imu.Arduino(port=port, baudrate=115200, timeout=0.1)
    app.run(host="0.0.0.0", port=5000, debug=True)
