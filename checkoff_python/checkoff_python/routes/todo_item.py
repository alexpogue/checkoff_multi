from bottle import Bottle, response, HTTPError
import json

from ..models.todo_item import TodoItem, TodoItemSchema

from .util import get_by_id, ensure_json_or_die, get_json_or_die, ensure_filled_or_die

todo_item_sub_app = Bottle()


@todo_item_sub_app.route('/')
def get_todo_items():
    all_todo_items = TodoItem.select()
    all_todo_items_json = TodoItemSchema().dump(all_todo_items, many=True)
    response.content_type = 'application/json'
    return json.dumps({'status': 'success', 'data': all_todo_items_json})


@todo_item_sub_app.route('/<todo_item_id:int>')
def get_todo_item(todo_item_id):
    todo_item = get_by_id(TodoItem, todo_item_id, TodoItemSchema())
    response.content_type = 'application/json'
    return json.dumps({'status': 'success', 'data': todo_item})


@todo_item_sub_app.route('/', method='POST')
def new_todo_item():
    ensure_json_or_die()
    request_data = get_json_or_die()

    title = request_data.get('title')
    details = request_data.get('details')
    is_complete = request_data.get('isComplete')

    ensure_filled_or_die(title=title, details=details, is_complete=is_complete)

    todo_item = TodoItem(title=title, details=details, isComplete=is_complete)
    todo_item.save()

    todo_item_json = TodoItemSchema().dump(todo_item)

    return json.dumps({'status': 'success', 'data': todo_item_json})


@todo_item_sub_app.route('/<todo_item_id:int>', method='PUT')
def update_todo_item(todo_item_id):
    ensure_json_or_die()
    request_data = get_json_or_die()

    try:
        todo_item = TodoItem.get_by_id(todo_item_id)
    except TodoItem.DoesNotExist:
        raise HTTPError(404)

    new_title = request_data.get('title')
    if new_title is not None:
        todo_item.title = new_title

    new_details = request_data.get('details')
    if new_details is not None:
        todo_item.details = new_details

    new_is_complete = request_data.get('isComplete')
    if new_is_complete is not None:
        todo_item.isComplete = new_is_complete

    todo_item.save()
    return json.dumps({'status': 'success'})


@todo_item_sub_app.route('/<todo_item_id:int>', method='DELETE')
def delete_todo_item(todo_item_id):
    try:
        todo_item = TodoItem.get_by_id(todo_item_id)
    except TodoItem.DoesNotExist:
        raise HTTPError(404)

    todo_item.delete_instance()
    return json.dumps({'status': 'success'})
