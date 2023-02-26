from flask import Flask, render_template, request, redirect, url_for, session
from pymongo import MongoClient
from dotenv import load_dotenv
import pytz

import os
import math
from datetime import datetime
from uuid import uuid4

if os.path.isfile('.env'):
    load_dotenv()

app = Flask(__name__)

app.config.from_object(dict(os.environ))
app.secret_key = os.environ.get('SECRET_KEY')

db = MongoClient(os.environ.get('MONGO_URI'))['overspeed']
admins = db['admins']
querries = db['querries']
reports = db['reports']

lats = [28.6300, 28.6309, 28.7087, 28.6849, 28.6524, 28.6499, 28.6557, 28.6460, 28.5990, 28.6924]
lons = [77.2142, 77.2255, 77.1181, 77.2026, 77.1929, 77.2175, 77.2190, 77.2081, 77.1241, 77.1738]
stations = ["station_1", "station_2", "station_3", "station_4", "station_5", "station_6", "station_7", "station_8", "station_9", "station_10"]

def closest(lat, lon):
    shortest = math.sqrt((lat - lats[0])**2 + (lon - lons[0])**2)
    shortest_index = 0
    for i in range(len(lats)):
        dist = math.sqrt((lat - lats[i])**2 + (lon - lons[i])**2)
        if dist < shortest:
            shortest = dist
            shortest_index = i
    return stations[shortest_index]

def add_report(lat, lon, speed, car_no):
    station = closest(lat, lon)
    reports.insert_one({
        "_id": str(uuid4()),
        "station": station,
        "lat": lat,
        "lon": lon,
        "speed": speed,
        "car_no": car_no,
        "resolved": False,
        "time": datetime.now(pytz.timezone('Asia/Kolkata')).strftime("%d/%m/%Y %H:%M:%S")
    })
    return station

@app.route('/')
def index():
    return render_template('index.html', user=session['user'] if 'user' in session else None)

@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        username = request.form['username']
        password = request.form['password']
        found = admins.find_one({'_id': username, 'pass': password})
        if found:
            session['user'] = found
            return redirect(url_for('dashboard'))
        return redirect(url_for('index'))
    return render_template('login.html')

@app.route('/logout')
def logout():
    session.pop('user', None)
    return redirect(url_for('index'))

@app.route('/dashboard')
def dashboard():
    if 'user' in session:
        return render_template('dashboard.html', user=session['user'])
    return redirect(url_for('index'))

@app.route('/reports')
def reports_point():
    if 'user' in session:
        station = request.args.get('station')
        if station:
            if station in stations and station in session['user']['access']:
                return render_template('reports.html', user=session['user'], reports=[i for i in reports.find({'station': station})], station=station)
        return redirect(url_for('dashboard'))
    return redirect(url_for('index'))

@app.route('/report')
def report_point():
    args = dict(request.args)
    if 'lat' in args and 'lon' in args and 'speed' in args and 'car' in args:
        st = add_report(float(args['lat']), float(args['lon']), float(args['speed']), args['car'])
        return 'OK: reported to ' + st
    return 'ERROR'

@app.route('/resolve')
def resolve_point():
    if 'user' in session:
        id = request.args.get('id')
        if id:
            reports.update_one({'_id': id}, {'$set': {'resolved': True}})
        return redirect(url_for('dashboard'))
    return redirect(url_for('index'))

if __name__ == '__main__':
    app.run(debug=True)