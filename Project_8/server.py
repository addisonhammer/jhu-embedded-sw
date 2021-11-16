from time import sleep
from flask import Flask, render_template, Response, render_template_string, session, copy_current_request_context
import imu
from concurrent.futures import ThreadPoolExecutor, Future
from flask_socketio import SocketIO, emit, disconnect
from threading import Lock

async_mode = None
app = Flask(__name__)
socket_ = SocketIO(app, async_mode=async_mode)

@app.route("/gps")
def get_gps():
    return "<p>Under Construction :(</p>"

@app.route("/imu")
def get_imu():
    return imu.ARDUINO.read_data().to_json()

@app.route("/video")
def get_video():
    return "<p>Under Construction :(</p>"

@app.route('/')
def index():
    return render_template('index.html', async_mode=socket_.async_mode)


@socket_.on('imu_request', namespace='/test')
def test_message():
    session['stop'] = False
    while not session['stop']:
        emit('response', {'data': get_imu()})
        sleep(0.2)

@socket_.on('disconnect_request', namespace='/test')
def disconnect_request():
    @copy_current_request_context
    def can_disconnect():
        disconnect()

    session['stop'] = True
    emit('response',
         {'data': 'Disconnected!'},
         callback=can_disconnect)

if __name__ == "__main__":
    socket_.run(app, host="0.0.0.0", port=5000, debug=True)