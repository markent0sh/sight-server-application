#include "MongoObject.hpp"
#include "Logger.hpp"

namespace service::storage::mongo {

service::storage::base::StorageError MongoObject::AddPair(
  const std::string& key,
  service::storage::base::IObject::Value value) {
  service::storage::base::StorageError error = service::storage::base::StorageError::eOk;
  SPDLOG_TRACE("AddPair {}: {}", key, static_cast<int>(value.first));

  try {
    switch(value.first) {
      case service::storage::base::StorageDataTypes::eInt:
        document_.append(bsoncxx::builder::basic::kvp(key, std::stol(std::get<std::string>(value.second))));
        break;
      case service::storage::base::StorageDataTypes::eFloat:
        document_.append(bsoncxx::builder::basic::kvp(key, std::stof(std::get<std::string>(value.second))));
        break;
      case service::storage::base::StorageDataTypes::eString:
        document_.append(bsoncxx::builder::basic::kvp(key, std::get<std::string>(value.second)));
        break;
      case service::storage::base::StorageDataTypes::eObject: {
        auto view = std::any_cast<bsoncxx::document::view>(
          std::get<std::unique_ptr<service::storage::base::IObject>>(value.second)->GetObject());
        document_.append(bsoncxx::builder::basic::kvp(key, view));
        break;
      }
      case service::storage::base::StorageDataTypes::eArray: {
        auto array = std::move(std::get<std::vector<std::unique_ptr<service::storage::base::IObject>>>(value.second));
        bsoncxx::builder::basic::array bson_array {};

        for(auto&& item : array) {
          bson_array.append(
            std::any_cast<bsoncxx::document::view>(item->GetObject()));
        }

        value.second = std::move(array);
        document_.append(bsoncxx::builder::basic::kvp(key, bson_array.extract()));
        break;
      }
      default:
        SPDLOG_WARN("AddPair, Unknown data type: {}", static_cast<int>(value.first));
        error = service::storage::base::StorageError::eNok;
        break;
    }

    if(service::storage::base::StorageError::eOk == error) {
      values_.emplace_back(key, std::move(value));
    }

  } catch(...) {
    SPDLOG_ERROR("AddPair, Exception from bsoncxx::append");
    error = service::storage::base::StorageError::eNok;
  }

  return error;
}

std::any MongoObject::GetObject() {
  return document_.view();
}

std::optional<service::storage::base::IObject::Values> MongoObject::GetObjectValues() {
  SPDLOG_TRACE("MongoObject::GetObjectValues, size: {}", values_.size());

  if(false == values_.empty()) {
    return std::move(values_);
  }

  return std::nullopt;
}

} // service::storage::mongo
