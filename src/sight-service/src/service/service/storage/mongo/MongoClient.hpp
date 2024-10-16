#pragma once

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/json.hpp>

#include "StorageClient.hpp"

namespace service::storage::mongo {

class MongoClient : public service::storage::base::IStorageClient {
  public:
    explicit MongoClient(
      const std::string& uri,
      const std::string& db_name,
      const std::string& collection);

    service::storage::base::StorageError Create(
     std::unique_ptr<service::storage::base::IObject> object) override;
    service::storage::base::StorageError Read(
     std::unique_ptr<service::storage::base::IObject> query,
     std::vector<std::unique_ptr<service::storage::base::IObject>>& values) override;
    service::storage::base::StorageError Update(
     std::unique_ptr<service::storage::base::IObject> filter,
     std::unique_ptr<service::storage::base::IObject> object) override;
    service::storage::base::StorageError Delete(
     std::unique_ptr<service::storage::base::IObject> filter) override;
  private:
    service::storage::base::StorageDataTypes Convert(const bsoncxx::type& type);
    std::string Convert(const bsoncxx::document::element& element);

    mongocxx::client client_;
    mongocxx::database db_;
    mongocxx::collection collection_;
};

} // service::storage::mongo
