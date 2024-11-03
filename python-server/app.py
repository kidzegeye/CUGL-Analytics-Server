from flask import Flask, request
from flask_socketio import SocketIO, emit
import os

app = Flask(__name__)
app.config['SECRET_KEY'] = os.getenv('FLASK_SECRET_KEY')
socketio = SocketIO(app, debug=True, cors_allowed_origins="*", async_mode="eventlet")


@app.route('/greeting/<name>')
def give_greeting(name):
    return 'Hello, {0}!'.format(name)


# You probably need to define more functions here (for connect, disconnect, etc)
@socketio.on('my meaningful name', namespace='/endpoint')
def endpoint_socket():
    emit([1, 2, 3])


@socketio.on("my_event")
def checkping():
    for x in range(5):
        cmd = "ping -c 1 8.8.8.8|head -2|tail -1"
        listing1 = subprocess.run(cmd, stdout=subprocess.PIPE, text=True, shell=True)
        sid = request.sid
        emit("server", {"data1": x, "data": listing1.stdout}, room=sid)
        socketio.sleep(1)


if __name__ == '__main__':
    socketio.run(app, host=os.getenv("HOST"), port=8000, debug=os.getenv("DEBUG"))
