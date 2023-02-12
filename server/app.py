from flask import Flask, request

app = Flask(__name__)

@app.route("/", methods = ["GET", "POST"])
def generic_handler():
    if request.method == "POST":
        print(request.form["temp"])
    return {"value": 10}
