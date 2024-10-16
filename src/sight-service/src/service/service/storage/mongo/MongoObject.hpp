#pragma once

#include <map>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/json.hpp>

#include "StorageObject.hpp"

namespace service::storage::mongo {

class MongoObject: public service::storage::base::IObject {
  public:
    service::storage::base::StorageError AddPair(
      const std::string& key,
      service::storage::base::IObject::Value value) override;
    std::any GetObject() override;
    std::optional<service::storage::base::IObject::Values> GetObjectValues() override;
  private:
    bsoncxx::builder::basic::document document_;
    service::storage::base::IObject::Values values_;
    std::map<std::string, bsoncxx::builder::basic::array> arrays_;
};

} // service::storage::mongo
