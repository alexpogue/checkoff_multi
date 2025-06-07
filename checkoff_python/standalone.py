from checkoff_python.server import app
from checkoff_python.init_single_threaded import init_databases
from waitress import serve


if __name__ == '__main__':
    init_databases()
    port = 3000
    print(f'running on port {port}...')
    serve(app, host='0.0.0.0', port=port)
