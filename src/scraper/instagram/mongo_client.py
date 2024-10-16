from pymongo import MongoClient
from enum import Enum
import logging
import config

class MongoClientWrapper:
  def __init__(self, database : str, collection : str) -> None:
    self.client_ = MongoClient('mongodb://localhost:27017/')
    self.db_ = self.client_[database]
    self.collection_ = self.db_[collection]

  def open_session(self):
    return self.client_.start_session()

  def close_session(self, session):
    session.end_session()

  def create(self, document, session):
    result = None

    try:
      result = self.collection_.insert_one(document, session = session)
    except Exception as error:
      logging.error(f'Unable to create document: {error.__str__()}')
      result = None

    return result

  def read(self, query, session):
    result = None

    try:
      result = list(self.collection_.find(query, session = session))
    except Exception as error:
      logging.error(f'Unable to read document: {error.__str__()}')
      result = None

    return result

  def update(self, query, document, session):
    result = None

    try:
      result = self.collection_.update_many(query, {'$set': document}, session = session)
    except Exception as error:
      logging.error(f'Unable to update document: {error.__str__()}')
      result = None

    return result

  def delete(self, query, session):
    result = None

    try:
      result = self.collection_.delete_many(query, session = session)
    except Exception as error:
      logging.error(f'Unable to delete document: {error.__str__()}')
      result = None

    return result

class DBError(Enum):
  eOK = 0,
  eNOK = 1

class AccountDocument:
  def __init__(self,
    login : str,
    password : str,
    status : bool,
    active : bool
  ):
    self.login_ = login
    self.password_ = password
    self.status_ = status
    self.active_ = active

class AccountsDB:
  def __init__(self) -> None:
    self.client_ = MongoClientWrapper(
      config.ACCOUNTS_DB_NAME,
      config.ACCOUNTS_COLLECTION_NAME)

  def get_all_accounts(self):
    accounts = None

    try:
      session = self.client_.open_session()
      accounts = self.client_.read({}, session)
      self.client_.close_session(session)
    except Exception as error:
      logging.error(f'AccountsDB :: Unable to get all accounts, error: {str(error)}')

    return accounts

  def update_account_status(self, login : str, update : bool) -> DBError:
    error : DBError = DBError.eOK

    try:
      session = self.client_.open_session()
      self.client_.update(
        {'login' : login},
        {'status' : update}, session)
      self.client_.close_session(session)
    except Exception as ex:
      logging.error(f'AccountsDB :: Unable to update account {login} status to {update}, error: {ex}')
      error = DBError.eNOK

    return error

  def update_account_active(self, login : str, update : bool) -> DBError:
    error : DBError = DBError.eOK

    try:
      session = self.client_.open_session()
      self.client_.update(
        {'login' : login},
        {'active' : update}, session)
      self.client_.close_session(session)
    except Exception as ex:
      logging.error(f'AccountsDB :: Unable to update account {login} active to {update}, error: {ex}')
      error = DBError.eNOK

    return error