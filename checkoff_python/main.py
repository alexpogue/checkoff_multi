from bottle import run
import checkoff_python

app = checkoff_python.create_app()


@app.route("/")
def hello():
    return "hello world!"


if __name__ == '__main__':
    run(app, host='0.0.0.0', port=3000, debug=True)
