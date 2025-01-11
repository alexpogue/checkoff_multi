# Required for using gunicorn from a distroless image since the `gunicorn` shell command does not work in its shell
# Mostly copied from: https://dev.to/lionelmarco/how-to-add-flask-gunicorn-packages-to-a-distroless-docker-container-2ml2

# file:standalone.py
from checkoff_python import server
from checkoff_python.server import app
import multiprocessing
import gunicorn.app.base

def number_of_workers():
    return  (multiprocessing.cpu_count() * 2) + 1

class StandaloneApplication(gunicorn.app.base.BaseApplication):

    def __init__(self, app, options=None):
        self.options = options or {}
        self.application = app
        super().__init__()

    def load_config(self):
        config = {key: value for key, value in self.options.items()
                  if key in self.cfg.settings and value is not None}
        for key, value in config.items():
            self.cfg.set(key.lower(), value)

    def load(self):
        return self.application

if __name__ == '__main__':
    options = {
        'bind': '%s:%s' % ('0.0.0.0', '3000'),
        'workers': number_of_workers(),
    }
    StandaloneApplication(app, options).run()

