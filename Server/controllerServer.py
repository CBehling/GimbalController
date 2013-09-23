from flask import Flask, render_template, jsonify, request
from GimbalUSB import GimbalUSB

gimbal = GimbalUSB()
app = Flask(__name__)

@app.route('/')
def home():
    return render_template('ui.html')

@app.route('/_position')
def _position():
	print "save me"
	pan = request.args.get('pan', 0, type=int)
	print pan
	tilt = request.args.get('tilt', 0, type=int)
	print tilt
	gimbal.set_pos(pan,tilt)
	return jsonify(result=0)
    #send commands to the servo here

if __name__ == '__main__':
    app.run()