from instagrapi import Client
from instagrapi.exceptions import LoginRequired
from enum import Enum
import logging
import random

class GramError(Enum):
  eOK = 0,
  eNOK = 1

# Global Notes:
# Consider removing caching if needed
# Improve mobile device behaviour emulation

class Gram:
  def __init__(self, cache_path) -> None:
    self.cache_path_ = cache_path
    self.client_ = None

  def login(self, username, password, proxy_url, challenge_cb):
    logging.info(f'Login {username} - {password} - {proxy_url}')
    result = GramError.eOK

    if None == self.client_:
      try:
        self.client_ = Client(proxy=proxy_url)
        self.client_.change_password_handler = challenge_cb
        self.client_.challenge_code_handler = lambda x, y: challenge_cb(x)
        session = None
        session_path = f'{self.cache_path_}/{username}-session.json'

        try:
          session = self.client_.load_settings(session_path)
        except Exception as error:
          logging.warning(f'No session file found for {username}, error: {error.__str__()}')
          session = None

        login_via_session = False
        login_via_password = False

        if None != session:
          try:
            self.client_.set_settings(session)
            self.client_.login(username, password)

            try:
              self.client_.get_timeline_feed()
            except LoginRequired:
              logging.info('Session is invalid, need to relogin via username and password')

              old_session = self.client_.get_settings()

              self.client_.set_settings({})
              self.client_.set_uuids(old_session['uuids'])

              self.client_.login(username, password)
              self.client_.delay_range = [1, random.randint(5, 10)]
              self.client_.dump_settings(session_path)

            logging.info(f'Logged {username} in via session')
            login_via_session = True
          except Exception as error:
            logging.info("Couldn't login user using session, error: %s" % error)

        if not login_via_session:
          try:
            logging.info(f'Attempting to login {username} via password')

            if True == self.client_.login(username, password):
              login_via_password = True
              self.client_.delay_range = [1, random.randint(5, 10)]
              self.client_.dump_settings(session_path)

          except Exception as e:
            logging.info("Couldn't login user using password, error: %s" % e)

        if not login_via_password and not login_via_session:
          logging.error(f'Login failed')
          result = GramError.eNOK
          self.client_ = None

      except Exception as error:
          logging.error("Login failed, error: %s" % error)
          result = GramError.eNOK
          self.client_ = None

    else:
      logging.warning(f'Already logged in')

    return result

  def get_target_following(self, target):
    logging.info(f'Get {target} following')
    kCountThreshold = 10000
    result = None

    if None != self.client_:
      try:
        user_info = self.client_.user_info_by_username(target.lower(), False)

        if user_info.following_count > kCountThreshold:
          logging.error(f'Get {target} following failed, threshold {kCountThreshold} exceeded: {user_info.following_count}')
        elif True == user_info.is_private:
          logging.error(f'Get {target} followers failed, profile is private')
        else:
          following = self.client_.user_following(user_id = str(user_info.pk), use_cache = False)
          result = {'following' : []}

          for key in following:
            follow = following[key]
            following_data = {
              'username' : f'{follow.username}',
              'name' : f'{follow.full_name}',
              'pic_url' : f'{follow.profile_pic_url}',
            }

            result['following'].append(following_data)

          received_len = len(result['following'])

          if user_info.following_count != received_len and user_info.following_count - 1 != received_len:
            logging.error(f"Failed to get full following list, expected: {user_info.following_count}, received: {received_len}")
            result = None

      except Exception as error:
        logging.error(f'Get {target} following failed, error: {error.__str__()}')
        result = None
    else:
      logging.error('Client is not ready')

    return result

  def get_target_followers(self, target):
    logging.info(f'Get {target} followers')
    kCountThreshold = 10000
    result = None

    if None != self.client_:
      try:
        user_info = self.client_.user_info_by_username(target.lower(), False)

        if user_info.follower_count > kCountThreshold:
          logging.error(f'Get {target} followers failed, threshold {kCountThreshold} exceeded: {user_info.follower_count}')
        elif True == user_info.is_private:
          logging.error(f'Get {target} followers failed, profile is private')
        else:
          followers = self.client_.user_followers(user_id = str(user_info.pk), use_cache = False)
          result = {'followers' : []}

          for key in followers:
            follower = followers[key]
            followers_data = {
              'username' : f'{follower.username}',
              'name' : f'{follower.full_name}',
              'pic_url' : f'{follower.profile_pic_url}',
            }

            result['followers'].append(followers_data)

          received_len = len(result['followers'])

          if user_info.follower_count != received_len and user_info.follower_count - 1 != received_len:
            logging.error(f"Failed to get full followers list, expected: {user_info.follower_count}, received: {received_len}")
            result = None

      except Exception as error:
        logging.error(f'Get {target} followers failed, error: {error.__str__()}')
        result = None
    else:
      logging.error('Client is not ready')

    return result

  def get_target_stories(self, target):
    logging.info(f'Get {target} stories')
    result = None

    if None != self.client_:
      try:
        user_info = self.client_.user_info_by_username(target.lower())

        if True == user_info.is_private:
          logging.error(f'Get {target} stories failed, profile is private')
        else:
          stories = self.client_.user_stories(str(user_info.pk))
          result = {'stories' : []}

          for story in stories:
            story_data = {
              'id' : f'{story.pk}',
              'video_url' : f'{story.video_url}',
              'photo_url' : f'{story.thumbnail_url}',
              'timestamp' : f'{story.taken_at}',
              'stickers' : []
            }

            for sticker in story.stickers:
              story_data['stickers'].append({
                'direct_url' : f'{sticker.story_link.url}',
                'display_url' : f'{sticker.story_link.display_url}'
              })

            result['stories'].append(story_data)

      except Exception as error:
        logging.error(f'Get {target} stories failed, error: {error.__str__()}')
        result = None
    else:
      logging.error('Client is not ready')

    return result

  def download_target_stories(self, target, stories, path):
    logging.info(f'Download {target} stories {stories} to the {path}')
    result = None

    if None != self.client_:
      try:
        result = {'stories' : []}

        for pk in stories:
          uri = self.client_.story_download(
            story_pk = pk,
            folder = path
          )

          result['stories'].append({
            'id' : f'{pk}',
            'uri' : f'{uri}'
          })

      except Exception as error:
        logging.error(f'Download {target} stories failed, error: {error.__str__()}')
        result = None
    else:
      logging.error('Client is not ready')

    return result

  def get_user_info(self, target):
    logging.info(f'Get {target} info')
    result = None

    if None != self.client_:
      try:
        user_info = self.client_.user_info_by_username(target.lower())
        result = {
          'username' : user_info.username,
          'private' : user_info.is_private,
          'follower-count' : user_info.follower_count,
          'following-count' : user_info.following_count,
          'post-count' : user_info.media_count,
          'phone' : user_info.public_phone_number,
          'email' : user_info.public_email
        }
      except Exception as error:
        logging.error(f'Get {target} stories info, error: {error.__str__()}')
        result = None
    else:
      logging.error('Client is not ready')

    return result
