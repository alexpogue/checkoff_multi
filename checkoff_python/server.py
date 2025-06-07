from checkoff_python import create_app

app = create_app()

if __name__ == "__main__":
    # start Bottle development server within container
    run(app, host='0.0.0.0', port=3000)
