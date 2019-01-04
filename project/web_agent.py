from pymongo import MongoClient
import json
client = MongoClient('localhost',27017)
db = client.RS
collect = db.toilet


def updateDB(index1,index2,index3):
    collect.delete_many({})
    data = writeData(index1, index2, index3)
    collect.insert_one(data)


test = [1,2,3,4]
#pre_data = {"name":'yanying','province':'江苏','age':25}
updateDB("abc", 98.78, test)

for post in collect.find():
    print(post)