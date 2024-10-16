from pymongo import MongoClient
import os
import sys
import json
import random

sys.path.append(
  os.path.join(
    os.path.dirname(os.path.realpath(__file__)), '../'))

import config

def create_accounts_from_json(path):
  client = MongoClient('mongodb://localhost:27017/')
  db = client[config.ACCOUNTS_DB_NAME]
  collection = db[config.ACCOUNTS_COLLECTION_NAME]

  proxies = [
    'http://sp47qm76x7:xr9jSgs28yz0Ur_fQL@de.smartproxy.com:20001',
    'http://sp47qm76x7:xr9jSgs28yz0Ur_fQL@de.smartproxy.com:20002',
    'http://sp47qm76x7:xr9jSgs28yz0Ur_fQL@de.smartproxy.com:20003',
    'http://sp47qm76x7:xr9jSgs28yz0Ur_fQL@de.smartproxy.com:20004',
    'http://sp47qm76x7:xr9jSgs28yz0Ur_fQL@de.smartproxy.com:20005',
    'http://sp47qm76x7:xr9jSgs28yz0Ur_fQL@de.smartproxy.com:20006',
    'http://sp47qm76x7:xr9jSgs28yz0Ur_fQL@de.smartproxy.com:20007',
    'http://sp47qm76x7:xr9jSgs28yz0Ur_fQL@de.smartproxy.com:20008',
    'http://sp47qm76x7:xr9jSgs28yz0Ur_fQL@de.smartproxy.com:20009',
    'http://sp47qm76x7:xr9jSgs28yz0Ur_fQL@de.smartproxy.com:20010'
  ]

  accounts = []
  with open(path, 'r') as file:
    accounts = json.load(file)

  documents = []
  for account in accounts['accounts']:
    print(account)
    read = list(collection.find({
      'login' : account.get('login')
    }))

    if len(read) > 0 or account in documents:
      print('Account already in db: {}'.format(account))
      continue
    else:
      account['proxy'] = random.choice(proxies)
      documents.append(account)

  if len(documents) > 0:
    collection.insert_many(documents)

def remove_accounts(accounts):
  client = MongoClient('mongodb://localhost:27017/')
  db = client[config.ACCOUNTS_DB_NAME]
  collection = db[config.ACCOUNTS_COLLECTION_NAME]

  if None is accounts:
    collection.delete_many({})
  else:
    for account in accounts:
      collection.delete_one({
        'login' : account
      })

def activate_accounts(accounts):
  client = MongoClient('mongodb://localhost:27017/')
  db = client[config.ACCOUNTS_DB_NAME]
  collection = db[config.ACCOUNTS_COLLECTION_NAME]

  for account in accounts:
    collection.update_one({'login' : account}, {'$set': {'active' : True}})

def free_accounts(accounts):
  client = MongoClient('mongodb://localhost:27017/')
  db = client[config.ACCOUNTS_DB_NAME]
  collection = db[config.ACCOUNTS_COLLECTION_NAME]

  for account in accounts:
    collection.update_one({'login' : account}, {'$set': {'status' : True}})

  if len(accounts) == 0:
    collection.update_many({}, {'$set': {'status' : True}})

def transform_accounts(donor, destination):
  accounts = []

  with open(donor, 'r') as file:
    for line in file:
      parts = line.strip().split(':')

      if len(parts) < 4:
        continue

      login = parts[0]
      password = parts[1]
      email = parts[2]
      email_password = parts[3]

      account = {
        "login": login,
        "password": password,
        "email" : email,
        "email-password" : email_password,
        "status": True,
        "active": True
      }
      accounts.append(account)

  data = {"accounts": accounts}
  with open(destination, 'w') as json_f:
    json.dump(data, json_f, indent=2)

def print_deactivated_accounts():
  client = MongoClient('mongodb://localhost:27017/')
  db = client[config.ACCOUNTS_DB_NAME]
  collection = db[config.ACCOUNTS_COLLECTION_NAME]

  accounts = list(collection.find({'active' : False}))
  for account in accounts:
    print('///////////////////////////////////')
    print(f"INST: {account.get('login')} :: {account.get('password')}")
    print(f"EMAIL: {account.get('email')} :: {account.get('email-password')}")

if __name__ == "__main__":
  remove_accounts(None)
  create_accounts_from_json('/home/mark-ssd/code/mdb/src/scraper/instagram/scripts/accounts.json')
  # free_accounts(['nandanyygg'])
  free_accounts([])
  # activate_accounts(['zyaoood74'])
  # print_deactivated_accounts()
  # transform_accounts(
  #   '/home/mark-ssd/code/mdb/src/scraper/instagram/scripts/z2u-NeoGenie-inst-6.txt',
  #   '/home/mark-ssd/code/mdb/src/scraper/instagram/scripts/accounts.json')
  # create_accounts_from_json('/home/mark-ssd/code/mdb/src/scraper/instagram/scripts/accounts.json')