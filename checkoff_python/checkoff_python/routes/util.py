from flask import abort, request


def get_by_id(model_cls, lookup_id, schema):
    try:
        model_obj = model_cls.get_by_id(lookup_id)
    except model_cls.DoesNotExist:
        abort(404)
    result = schema.dump(model_obj)
    return result


def ensure_json_or_die():
    if not request.is_json:
        print(
            'Error: {} {} only supports Content-Type: application/json'
            .format(request.method, request.path)
        )
        abort(415, {'message': 'Expected Content-Type: application/json'})
