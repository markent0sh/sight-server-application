import requests
import json
from instagrapi import Client

FLASK_SERVER_URL = "http://localhost:5000"

def setup_server():
  url = f"{FLASK_SERVER_URL}/setup"
  response = requests.post(url)
  print(f"Server setup response: {response.text}")

def shutdown_server():
  url = f"{FLASK_SERVER_URL}/shutdown"
  response = requests.post(url)
  print(f"Server shutdown response: {response.text}")

def get_stories(payload):
  url = f"{FLASK_SERVER_URL}/get_stories"
  response = requests.post(url, json=payload)
  print(f"Get Stories response: \n{response.text}")

  return json.loads(response.text)

def download_stories(payload):
  url = f"{FLASK_SERVER_URL}/download_stories"
  response = requests.post(url, json=payload)
  print(f"Download Stories response: {response.text}")

  return json.loads(response.text)

def main():
  get_stories_request = {
    "target" : "obladaet"
  }

  download_stories_request = {
    "target" : "obladaet",
    "stories" : []
  }

  setup_server()
  get_result = get_stories(json.dumps(get_stories_request))

  for story in get_result['stories']:
    download_stories_request['stories'].append(story['id'])

  # download_stories(json.dumps(download_stories_request))

  # cl = Client()
  # cl.login('kggkk90', 'WbDcbinzj4098')

  # user_id = cl.user_id_from_username('obladaet')
  # stories = cl.user_stories(user_id)

  # for story in stories:
  #   print('----------------------------')
  #   print(story)
  #   print('----------------------------')
  #   print(f'Story ID: {story.id}')
  #   print(f'Video URL: {story.video_url}')
  #   print(f'Thumbnail URL: {story.thumbnail_url}')
  #   print(f'Timestamp: {story.taken_at}')
  #   print(f'Links: {story.links}')
  #   print(f'Hashtags: {story.hashtags}')
  #   print(f'Locations: {story.locations}')
  #   for sticker in story.stickers:
  #     print(f'  Sticker Link URL: {sticker.story_link.url}')
  #     print(f'  Sticker Link Display URL: {sticker.story_link.display_url}')
  #   for mention in story.mentions:
  #     print(f'  Mention: {mention.user.username}')


if __name__ == "__main__":
  main()
