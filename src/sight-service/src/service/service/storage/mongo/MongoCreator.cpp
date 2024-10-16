#include "MongoCreator.hpp"

#include "MongoObject.hpp"
#include "MongoClient.hpp"

namespace service::storage::mongo {

std::optional<std::unique_ptr<service::storage::base::IObject>> MongoObjectCreator::Create() {
  std::unique_ptr<service::storage::base::IObject> result = nullptr;

  try {
    result = std::make_unique<service::storage::mongo::MongoObject>();
  } catch (...) {
    return std::nullopt;
  }

  return result;
}

MongoClientCreator::MongoClientCreator(
  const std::string& uri,
  const std::string& db_name,
  const std::string& collection)
  : uri_(uri),
    db_name_(db_name),
    collection_(collection) {
}

std::optional<std::shared_ptr<service::storage::base::IStorageClient>> MongoClientCreator::Create() {
  std::shared_ptr<service::storage::base::IStorageClient> result = nullptr;

  try {
    result = std::make_shared<service::storage::mongo::MongoClient>(uri_, db_name_, collection_);
  } catch (...) {
    return std::nullopt;
  }

  return result;
}

} // service::storage::mongo
