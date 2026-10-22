from flask import Flask
from flask_pymongo import PyMongo
from pymongo import MongoClient

app = Flask(__name__)
app.config["MONGO_URI"] = "mongodb://admin:pass@mongo:27017/weather_db?authSource=admin"

mongo = PyMongo(app)
client = MongoClient("mongodb://mongo:27017")

db = mongo.db
db.countries.create_index('nume', unique=True)
db.cities.create_index([('nume', 1), ('idTara', 1)], unique=True)
db.temperatures.create_index([('idOras', 1), ('timestamp', 1)], unique=True)

from app import routes
