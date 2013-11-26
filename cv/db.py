from flask import Flask, request
import json
import time
app = Flask(__name__)

fires = {}
ids = []


@app.route("/add_fire", methods=['GET', 'POST'])
def add_fire():
    try:
        x = request.args.get('x')
        y = request.args.get('y')
        id = request.args.get('id')

        x = int(x)
        y = int(y)
        id = int(id)
        t = int(time.time())
        fires[id]({'x': x, 'y': y, 't': t})
        ids.append([(id, t)])
        i = 0
        while len(ids) > 0 and time.time() - ids[i][1] > 10:
            if ids[i][t] == fires[ids[i][0]]['t']:
                del(fires[ids[i][0]])    
            del(fires[i])
        return "ho{}".format(len(fires))
    except:
        return "hi"


@app.route("/get_fires")
def hello():
    return json.dumps(fires)

if __name__ == "__main__":
        app.run(debug=True)
