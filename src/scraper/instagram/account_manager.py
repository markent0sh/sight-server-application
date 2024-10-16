from enum import Enum
import time
import logging
import threading

import mongo_client

class Account:
  def __init__(self, login, password, proxy) -> None:
    self.login = login
    self.password = password
    self.proxy = proxy

class AccountManagerError(Enum):
  eOK = 0,
  eNOK = 1

class AccountManager:
  def __init__(self) -> None:
    self.active_accounts = None
    self.accounts_db = mongo_client.AccountsDB()
    self.mutex = threading.Lock()

  def acquire_account(self) -> Account:
    result = Account(None, None, None)

    with self.mutex:

      if None != self.active_accounts:
        for account in self.active_accounts:

          if True is account['status'] and True is account['active']: # True: Account is free and active
            result.login = account['login']
            result.password = account['password']
            result.proxy = account['proxy']
            account['status'] = False
            db_error = self.accounts_db.update_account_status(account['login'], account['status'])

            if mongo_client.DBError.eOK is db_error:
              logging.info(f'AccountManager :: Acquired {result.login} : {result.password}')
              break
            else:
              result.login = None
              result.password = None

    return result

  def release_account(self, login) -> AccountManagerError:
    error = AccountManagerError.eOK

    with self.mutex:

      if None != self.active_accounts:
        for account in self.active_accounts:

          if login == account['login']:
            account['status'] = True
            db_error = self.accounts_db.update_account_status(account['login'], account['status'])

            if mongo_client.DBError.eOK is db_error:
              login = account['login']
              password = account['password']
              logging.info(f'AccountManager :: Released {login} : {password}')
            else:
              error = AccountManagerError.eNOK

            break
      else:
        error = AccountManagerError.eNOK

    return error

  def activate_account(self, login) -> AccountManagerError:
    error = AccountManagerError.eOK

    with self.mutex:

      if None != self.active_accounts:
        for account in self.active_accounts:

          if login == account['login']:
            account['active'] = True
            db_error = self.accounts_db.update_account_active(account['login'], account['active'])

            if mongo_client.DBError.eOK is db_error:
              login = account['login']
              password = account['password']
              logging.info(f'AccountManager :: Activated {login} : {password}')
            else:
              error = AccountManagerError.eNOK

            break

    return error

  def deactivate_account(self, login) -> AccountManagerError:
    error = AccountManagerError.eOK

    with self.mutex:

      if None != self.active_accounts:
        for account in self.active_accounts:

          if login == account['login']:
            account['active'] = False
            db_error = self.accounts_db.update_account_active(account['login'], account['active'])

            if mongo_client.DBError.eOK is db_error:
              login = account['login']
              password = account['password']
              logging.info(f'AccountManager :: Deactivated {login} : {password}')
            else:
              error = AccountManagerError.eNOK

            break
      else:
        error = AccountManagerError.eNOK

    return error

  def load(self) -> AccountManagerError:
    error = AccountManagerError.eOK

    with self.mutex:
      self.active_accounts = self.accounts_db.get_all_accounts()

      if None is self.active_accounts:
        error = AccountManagerError.eNOK

    return error

if __name__ == "__main__":
  logging.basicConfig(
    level = logging.DEBUG,
    format = '%(name)s - %(levelname)s - %(message)s'
  )

  manager = AccountManager()

  while True:
    account = manager.acquire_account()

    if None == account.login:
      break

    logging.info(f'Login: {account.login} | Password {account.password}')
    time.sleep(10)

  manager.deactivate_account('nandanyygg')
  time.sleep(10)

  manager.activate_account('nandanyygg')
  time.sleep(10)

  manager.release_account('nandanyygg')
