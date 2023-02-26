import sqlite3
from flask import Flask, request, render_template, redirect


app = Flask(__name__)
app.config["TEMPLATES_AUTO_RELOAD"] = True

conn = sqlite3.connect('arduino.db', check_same_thread=False)
cur = conn.cursor()


@app.route('/', methods=['GET'])
def history():
    cur.execute("SELECT * FROM access_log ORDER BY id DESC")
    access_log = cur.fetchall()
    return render_template("index.html", access_log = access_log)


@app.route('/getdata', methods=['GET'])
def get_data():
    row = request.args.getlist('data')
    cur.execute("INSERT INTO access_log (unix, date_time, card_id, first_name, last_name, comments)\
                VALUES(?, ?, ?, ?, ?, ?)", row)
    conn.commit()
    return redirect("/")


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)

