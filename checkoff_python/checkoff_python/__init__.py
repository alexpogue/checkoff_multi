from bottle import Bottle

import os


def create_app():
    print(f'PYTHONMALLOC = {os.getenv("PYTHONMALLOC")}')

    from . import models, routes
    app = Bottle()

    models.init_app()
    routes.init_app(app)
    return app
