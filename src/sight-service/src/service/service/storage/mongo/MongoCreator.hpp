#pragma once

#include "StorageCreator.hpp"

namespace service::storage::mongo {

class MongoObjectCreator : public service::storage::base::IObjectCreator {
  public:
    std::optional<std::unique_ptr<service::storage::base::IObject>> Create() override;
};

class MongoClientCreator : public service::storage::base::IClientCreator {
  public:
    explicit MongoClientCreator(
      const std::string& uri,
      const std::string& db_name,
      const std::string& collection);

    std::optional<std::shared_ptr<service::storage::base::IStorageClient>> Create() override;
  private:
    std::string uri_;
    std::string db_name_;
    std::string collection_;
};

} // service::storage::mongo
