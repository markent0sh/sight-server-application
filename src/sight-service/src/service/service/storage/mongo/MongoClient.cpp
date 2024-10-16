#include "MongoClient.hpp"
#include "MongoObject.hpp"
#include "Logger.hpp"

namespace service::storage::mongo {

MongoClient::MongoClient(
  const std::string& uri,
  const std::string& db_name,
  const std::string& collection)
  : client_(mongocxx::client{mongocxx::uri {uri}}),
    db_(client_[db_name]),
    collection_(db_[collection]) {
  SPDLOG_INFO("MongoClient::Construct: {} - {} - {}", uri, db_name, collection);
}

service::storage::base::StorageError MongoClient::Create(
  std::unique_ptr<service::storage::base::IObject> object) {
  service::storage::base::StorageError error = service::storage::base::StorageError::eOk;
  SPDLOG_DEBUG("MongoClient::Create");

  try {
    auto result = collection_.insert_one(std::any_cast<bsoncxx::document::view>(object->GetObject()));

    if(result) {
      SPDLOG_DEBUG("MongoClient::Create, Inserted document with id: {}", result->inserted_id().get_oid().value.to_string());
    } else {
      error = service::storage::base::StorageError::eNok;
    }

  } catch(...) {
    SPDLOG_ERROR("MongoClient::Create, exception from insert_one");
    error = service::storage::base::StorageError::eNok;
  }

  return error;
}

service::storage::base::StorageError MongoClient::Read(
  std::unique_ptr<service::storage::base::IObject> query,
  std::vector<std::unique_ptr<service::storage::base::IObject>>& values) {
  service::storage::base::StorageError error = service::storage::base::StorageError::eOk;
  SPDLOG_DEBUG("MongoClient::Read");

  try {
    auto result = collection_.find(std::any_cast<bsoncxx::document::view>(query->GetObject()));

    for(auto&& item : result) {
      SPDLOG_DEBUG("MongoClient::Read, Found matching document: {}", bsoncxx::to_json(item));

      auto object = std::make_unique<MongoObject>();
      for(auto&& element : item) {
        std::string key = element.key().to_string();
        service::storage::base::IObject::Value value;

        switch(element.type()) {
          case bsoncxx::type::k_utf8:
            value.first = service::storage::base::StorageDataTypes::eString;
            value.second = element.get_utf8().value.to_string();
            break;
          case bsoncxx::type::k_double:
            value.first = service::storage::base::StorageDataTypes::eFloat;
            value.second = std::to_string(element.get_double().value);
            break;
          case bsoncxx::type::k_int32:
            value.first = service::storage::base::StorageDataTypes::eInt;
            value.second = std::to_string(element.get_int32().value);
            break;
          case bsoncxx::type::k_int64:
            value.first = service::storage::base::StorageDataTypes::eInt;
            value.second = std::to_string(element.get_int64().value);
            break;
          case bsoncxx::type::k_document: {
            value.first = service::storage::base::StorageDataTypes::eObject;
            auto subobject = std::make_unique<MongoObject>();

            for(auto&& subitem : element.get_document().view()) {
              subobject->AddPair(
                subitem.key().to_string(),
                std::make_pair(Convert(subitem.type()), Convert(subitem)));
            }

            value.second = std::move(subobject);
            break;
          }
          case bsoncxx::type::k_array: {
            value.first = service::storage::base::StorageDataTypes::eArray;
            std::vector<std::unique_ptr<service::storage::base::IObject>> array;

            for(auto&& subitem : element.get_array().value) {
              auto subobject = std::make_unique<MongoObject>();

              for(auto&& docitem : subitem.get_document().view()) {
                subobject->AddPair(
                  docitem.key().to_string(),
                  std::make_pair(Convert(docitem.type()), Convert(docitem)));
              }

              array.push_back(std::move(subobject));
            }

            value.second = std::move(array);
            break;
          }
          default:
            SPDLOG_ERROR("MongoClient::Read, Unknown data type: {}", static_cast<int>(element.type()));
            break;
        }

        if(service::storage::base::StorageDataTypes::eDefault != value.first) {
          object->AddPair(key, std::move(value));
        }

      }
      values.push_back(std::move(object));

    }

  } catch(...) {
    SPDLOG_ERROR("MongoClient::Read, exception from find");
    error = service::storage::base::StorageError::eNok;
  }

  return error;
}

service::storage::base::StorageError MongoClient::Update(
  std::unique_ptr<service::storage::base::IObject> filter,
  std::unique_ptr<service::storage::base::IObject> object) {
  service::storage::base::StorageError error = service::storage::base::StorageError::eOk;
  SPDLOG_DEBUG("MongoClient::Update");

  try {
    auto updater = bsoncxx::builder::basic::document {};
    updater.append(bsoncxx::builder::basic::kvp(
      "$set",
      bsoncxx::document::value(std::any_cast<bsoncxx::document::view>(object->GetObject())))
    );

    auto result = collection_.update_many(
      std::any_cast<bsoncxx::document::view>(filter->GetObject()),
      updater.view());

    if(result) {
      SPDLOG_DEBUG("MongoClient::Update, Matched count: {}, Modified count: {}", result->matched_count(), result->modified_count());
    } else {
      error = service::storage::base::StorageError::eNok;
    }

  } catch(...) {
    SPDLOG_ERROR("MongoClient::Update, exception from update_many");
    error = service::storage::base::StorageError::eNok;
  }

  return error;
}

service::storage::base::StorageError MongoClient::Delete(
  std::unique_ptr<service::storage::base::IObject> filter) {
  service::storage::base::StorageError error = service::storage::base::StorageError::eOk;
  SPDLOG_DEBUG("MongoClient::Delete");

  try {
    auto result = collection_.delete_many(std::any_cast<bsoncxx::document::view>(filter->GetObject()));

    if(result) {
      SPDLOG_DEBUG("MongoClient::Delete, Deleted count: {}", result->deleted_count());
    } else {
      error = service::storage::base::StorageError::eNok;
    }

  } catch(...) {
    SPDLOG_ERROR("MongoClient::Delete, exception from delete_many");
    error = service::storage::base::StorageError::eNok;
  }

  return error;
}

service::storage::base::StorageDataTypes MongoClient::Convert(const bsoncxx::type& type) {
  switch(type) {
    case bsoncxx::type::k_utf8:
      return service::storage::base::StorageDataTypes::eString;
      break;
    case bsoncxx::type::k_double:
      return service::storage::base::StorageDataTypes::eFloat;
      break;
    case bsoncxx::type::k_int32:
    case bsoncxx::type::k_int64:
      return service::storage::base::StorageDataTypes::eInt;
      break;
    default:
      return service::storage::base::StorageDataTypes::eDefault;
      break;
  }
}

std::string MongoClient::Convert(const bsoncxx::document::element& element) {
  switch (element.type()) {
    case bsoncxx::type::k_utf8:
      return element.get_utf8().value.to_string();
    case bsoncxx::type::k_double:
      return std::to_string(element.get_double().value);
    case bsoncxx::type::k_int32:
      return std::to_string(element.get_int32().value);
    case bsoncxx::type::k_int64:
      return std::to_string(element.get_int64().value);
    case bsoncxx::type::k_bool:
      return element.get_bool().value ? "1" : "0";
    default:
      return "None";
  }
}

} // service::storage::mongo
