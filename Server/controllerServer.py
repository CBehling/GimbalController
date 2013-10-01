from flask import Flask, render_template, jsonify, request
from GimbalUSB import GimbalUSB

gimbal = GimbalUSB()
app = Flask(__name__)

@app.route('/')
def home():
    return render_template('ui.html')

@app.route('/_position')
def _position():
	pan = request.args.get('pan', 0, type=int)
	tilt = request.args.get('tilt', 0, type=int)
	gimbal.set_pos(pan,tilt)
	return jsonify()

@app.route('/_scan')
def _scan():
	print "hello"
	gimbal.scan()
	print "hello1"
	return jsonify()



if __name__ == '__main__':
    app.run()