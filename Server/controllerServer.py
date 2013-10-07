from flask import Flask, render_template, jsonify, request
from GimbalUSB import GimbalUSB
from time import sleep

gimbal = GimbalUSB()
app = Flask(__name__)

@app.route('/')
def home():
    return render_template('vis.html')

@app.route('/_position')
def _position():
	pan = request.args.get('pan', 0, type=float)
	tilt = request.args.get('tilt', 0, type=float)
	gimbal.set_pos(pan,tilt)
	return jsonify()

@app.route('/_ping')
def _ping():
	pan = request.args.get('pan', 0, type=float)
	tilt = request.args.get('tilt', 0, type=float)
	gimbal.set_pos(pan,tilt)
	sleep(.05)
	dist = gimbal.get_dist()
	return jsonify(pan=gimbal.pan, tilt=gimbal.tilt, dist=dist) 

@app.route('/_scan', methods=["GET"])
def _scan():
	#gimbal.scan()
	dist = gimbal.get_dist()
	return jsonify(pan=gimbal.pan, tilt=gimbal.tilt, dist=dist)



if __name__ == '__main__':
    app.run()