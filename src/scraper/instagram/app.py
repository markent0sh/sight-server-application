from flask import Flask, request
import os
import signal
import json
import logging

import gram_manager
import config

logging.basicConfig(
  filename = 'app.log',
  filemode = 'a',
  level = logging.INFO,
  format = '%(name)s - %(levelname)s - %(message)s'
)

grammer = None
app = Flask(__name__)

@app.route('/login', methods=['POST'])
def login():
  logging.info('/login')
  code = 200
  response = {}

  try:
    result = grammer.login()

    if None != result:
      response = json.dumps(result)
    else:
      code = 500

  except Exception as error:
    logging.error(f'Login failed, error: {error.__str__()}')

  return response, code

@app.route('/shutdown', methods=['POST'])
def shutdown():
  logging.info('/shutdown')
  code = 200
  response = {}
  os.kill(os.getpid(), signal.SIGINT)

  return response, code

@app.route('/get_stories', methods=['POST'])
def get_stories():
  logging.info('/get_stories')
  code = 200
  response = {}
  data = request.json

  try:
    target = data['target']
    result = grammer.get_target_stories(target)

    if None != result:
      response = json.dumps(result)
    else:
      code = 500

  except Exception as error:
    logging.error(f'Get stories failed, error: {error.__str__()}')

  return response, code

@app.route('/get_target_following', methods=['POST'])
def get_target_following():
  logging.info('/get_target_following')
  code = 200
  response = {}
  data = request.json

  try:
    target = data['target']
    result = grammer.get_target_following(target)

    if None != result:
      response = json.dumps(result)
    else:
      code = 500

  except Exception as error:
    logging.error(f'Get following failed, error: {error.__str__()}')

  return response, code

@app.route('/get_target_followers', methods=['POST'])
def get_target_followers():
  logging.info('/get_target_followers')
  code = 200
  response = {}
  data = request.json

  try:
    target = data['target']
    result = grammer.get_target_followers(target)

    if None != result:
      response = json.dumps(result)
    else:
      code = 500

  except Exception as error:
    logging.error(f'Get stories failed, error: {error.__str__()}')

  return response, code

@app.route('/download_stories', methods=['POST'])
def download_stories():
  logging.info('/download_stories')
  code = 200
  response = {}
  data = request.json

  try:
    target = data['target']
    stories = data['stories']
    result = grammer.download_target_stories(
      target,
      stories,
      config.LOCAL_STORAGE
    )

    if None != result:
      response = json.dumps(result)
    else:
      code = 500

  except Exception as error:
    logging.error(f'Get stories failed, error: {error.__str__()}')

  return response, code

if __name__ == "__main__":
  logging.info('App started')
  grammer = gram_manager.GramManager(config.CACHE_STORAGE)

  from waitress import serve
  serve(app, host=config.FLASK_ADDR, port = config.FLASK_PORT)
