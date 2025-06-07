from .todo_item import todo_item_sub_app


def init_app(app):
    app.mount('/todo-item', todo_item_sub_app)
