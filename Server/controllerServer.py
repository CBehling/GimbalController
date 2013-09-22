from flask import Flask, render_template
app = Flask(__name__)

@app.route('/')
def home():
    return render_template('ui.html')

@app.route('/_position')
def _position():
	x = request.args.get('x', 0, type=int)
	y = request.args.get('y', 0, type=int)
    #send commands to the servo here

if __name__ == '__main__':
    app.run()