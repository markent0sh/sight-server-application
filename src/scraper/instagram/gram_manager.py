import logging
import random
import threading
import time

import gram
import account_manager

class GramManager:
  def __init__(self, cache_path) -> None:
    self.grams = []
    self.grams_mutex = threading.Lock()
    self.cache_path = cache_path
    self.failed_grams = []
    self.account_manager = account_manager.AccountManager()
    self.is_login_ongoing = False

  def __del__(self):
    for item in self.grams:
      login = item['account'].login
      error = self.account_manager.release_account(login)

      if account_manager.AccountManagerError.eOK != error:
        logging.error(f'GramManager :: Failed to release {login}')

  def login(self):
    self.is_login_ongoing = True
    load_error = self.account_manager.load()

    if account_manager.AccountManagerError.eOK != load_error:
      logging.error(f'GramManager :: Failed load accounts {load_error}')
      return

    result_len = 0

    with self.grams_mutex: # App is blocked
      while True:
        account = self.account_manager.acquire_account()

        if None == account.login:
          break

        self.grams.append({
          'gram' : gram.Gram(self.cache_path),
          'account' : account,
          'status' : True
        })

      invalids = []
      for item in self.grams:
        error = item['gram'].login(
          item['account'].login,
          item['account'].password,
          item['account'].proxy,
          self._deactivate_gram
        )

        if gram.GramError.eOK != error:
          invalids.append(item['account'].login)
          logging.error(f'GramManager :: Failed to login')
        else:
          result_len = result_len + 1

      for invalid in invalids:
        gram_item = self._find_gram(invalid)

        if None != gram_item:
          self.grams.remove(gram_item)

        error = self.account_manager.deactivate_account(invalid)

        if account_manager.AccountManagerError.eOK != error:
          logging.error(f'GramManager :: Failed to deactivate {invalid}')

        error = self.account_manager.release_account(invalid)

        if account_manager.AccountManagerError.eOK != error:
          logging.error(f'GramManager :: Failed to release {invalid}')

    logging.info(f'GramManager :: Login completed :: {len(self.grams)} accounts acquired')
    self.is_login_ongoing = False
    return {'amount' : result_len}

  def get_target_following(self, target):
    result = None
    worker_index = self._lock_gram()

    if None is not worker_index:
      try:
        result = self.grams[worker_index]['gram'].get_target_following(target)
        self._unlock_gram(self.grams[worker_index]['account'].login)
      except Exception as error:
        logging.error(f'GramManager :: Get {target} following failed, error: {error}')

    return result

  def get_target_followers(self, target):
    result = None
    worker_index = self._lock_gram()

    if None is not worker_index:
      try:
        result = self.grams[worker_index]['gram'].get_target_followers(target)
        self._unlock_gram(self.grams[worker_index]['account'].login)
      except Exception as error:
        logging.error(f'GramManager :: Get {target} followers failed, error: {error}')

    return result

  def get_target_stories(self, target):
    result = None
    worker_index = self._lock_gram()

    if None is not worker_index:
      try:
        result = self.grams[worker_index]['gram'].get_target_stories(target)
        self._unlock_gram(self.grams[worker_index]['account'].login)
      except Exception as error:
        logging.error(f'GramManager :: Get {target} stories failed, error: {error}')

    return result

  def download_target_stories(self, target, stories, path):
    result = None
    worker_index = self._lock_gram()

    if None is not worker_index:
      try:
        result = self.grams[worker_index]['gram'].download_target_stories(target, stories, path)
        self._unlock_gram(self.grams[worker_index]['account'].login)
      except Exception as error:
        logging.error(f'GramManager :: Download {target} stories failed, error: {error}')

    return result

  def _get_random_safe(self, begin, end):
    index = None

    try:
      index = random.randint(begin, end)
    except Exception as error:
      logging.error(f'GramManager :: Get random safe failed {begin}:{end}, error: {error}')
      index = None

    return index

  def _find_gram(self, login):
    gram_item = None
    for item in self.grams:

      if login == item['account'].login:
        gram_item = item
        break

    return gram_item

  def _lock_gram(self):
    worker_index = None

    with self.grams_mutex:
      retries = len(self.grams)

      while retries > 0:
        worker_index = self._get_random_safe(0, len(self.grams) - 1)

        if None is not worker_index:

          if True is self.grams[worker_index]['status']:
            self.grams[worker_index]['status'] = False
            logging.info(f"GramManager :: locked {self.grams[worker_index]['account'].login}")
            break
          else:
            worker_index = None
            time.sleep(0.01)

        retries = retries - 1

    return worker_index

  def _unlock_gram(self, login):
    with self.grams_mutex:

      if login not in self.failed_grams:
        inactive = self._find_gram(login)
        inactive['status'] = True
        logging.info(f"GramManager :: unlocked {login}")
      else:
        logging.error(f"GramManager :: gram {login} unlock failed")
        logging.warning(f"GramManager :: we have {len(self.grams) - len(self.failed_grams)} remaining")

  def _deactivate_gram(self, login):
    logging.error(f'GramManager :: Deactivating {login} permanently, please procceed manualy')
    self.account_manager.deactivate_account(login)
    self.account_manager.release_account(login)

    if False == self.is_login_ongoing:
      with self.grams_mutex:
        inactive = self._find_gram(login)
        inactive['status'] = False
        self.failed_grams.append(login)

    return 0