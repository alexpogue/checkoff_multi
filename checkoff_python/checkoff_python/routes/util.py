from bottle import request, HTTPError


def get_by_id(model_cls, lookup_id, schema):
    try:
        model_obj = model_cls.get_by_id(lookup_id)
    except model_cls.DoesNotExist:
        raise HTTPError(404)
    result = schema.dump(model_obj)
    return result


def ensure_json_or_die():
    if not request.content_type or 'application/json' not in request.content_type:
        print(
            f'Error: {request.method} {request.path} only supports Content-Type: application/json'
        )
        raise HTTPError(415, 'Expected Content-Type: application/json')


def get_json_or_die():
    try:
        request_data = request.json
        if request_data is None:
            raise HTTPError(400, "No JSON body provided")
        return request_data
    except Exception as e:
        raise HTTPError(400, f"Invalid or malformed JSON body")


def ensure_filled_or_die(**kwargs):
    missing = [ key for key, value in kwargs.items() if value is None ]
    if missing:
        raise HTTPError(400, f"Missing required fields: {', '.join(missing)}")
