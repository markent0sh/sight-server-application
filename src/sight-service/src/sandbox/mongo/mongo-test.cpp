#include "MongoClient.hpp"
#include "MongoObject.hpp"

#include "Logger.hpp"

int main() {
  SET_LOG_LEVEL();

  service::storage::mongo::MongoClient client(
    std::string("mongodb://localhost:27017"),
    std::string("storage-test-db"),
    std::string("storage-test-collection"));

  // Create
  {
    auto mongo_object = std::make_unique<service::storage::mongo::MongoObject>();
    mongo_object->AddPair(
      std::string("user-id"),
      std::make_pair(service::storage::base::StorageDataTypes::eInt, std::string("66666")));

    std::vector<std::unique_ptr<service::storage::base::IObject>> array;
    for(auto i = 0; i < 5; i++) {
      auto sub_object = std::make_unique<service::storage::mongo::MongoObject>();
      sub_object->AddPair(
        std::string("some-key"),
        std::make_pair(service::storage::base::StorageDataTypes::eInt, std::to_string(i * 666)));
      sub_object->AddPair(
        std::string("other-key"),
        std::make_pair(service::storage::base::StorageDataTypes::eString, std::string("text-string")));
      array.push_back(std::move(sub_object));
    }

    mongo_object->AddPair(
      std::string("array"),
      std::make_pair(service::storage::base::StorageDataTypes::eArray, std::move(array)));

    client.Create(std::move(mongo_object));
  }

  // Find & Read
  {
    // std::vector<std::unique_ptr<service::storage::base::IObject>> results;
    // auto mongo_object = std::make_unique<service::storage::mongo::MongoObject>();
    // mongo_object->AddPair(
    //   std::string("task-id"),
    //   std::make_pair(service::storage::base::StorageDataTypes::eInt, std::string("12")));
    // client.Read(std::move(mongo_object), results);

    // SPDLOG_DEBUG("======== Read results ========");
    // for(auto&& item : results) {
    //   auto values = item->GetObjectValues();

    //   SPDLOG_DEBUG("--------");
    //   if(std::nullopt != values) {

    //     for(auto& value : values.value()) {
    //       SPDLOG_DEBUG("key: {}, value: {}", value.first, std::get<std::string>(value.second.second));
    //     }

    //   }

    // }
  }

  // Read All
  {
    std::vector<std::unique_ptr<service::storage::base::IObject>> results;
    auto mongo_object = std::make_unique<service::storage::mongo::MongoObject>();
    client.Read(std::move(mongo_object), results);

    SPDLOG_DEBUG("======== Read All results ========");
    for(auto&& item : results) {
      auto values = item->GetObjectValues();

      SPDLOG_DEBUG("--------");
      if(std::nullopt != values) {

        for(auto& value : values.value()) {

          if(service::storage::base::StorageDataTypes::eObject == value.second.first) {
            auto subobject = std::get<std::unique_ptr<service::storage::base::IObject>>(std::move(value.second.second));
            auto subvalues = subobject->GetObjectValues();

            for(auto& subvalue : subvalues.value()) {
              SPDLOG_DEBUG(" >>> key: {}, value: {}", subvalue.first, std::get<std::string>(subvalue.second.second));
            }

          } else if(service::storage::base::StorageDataTypes::eArray == value.second.first) {
            auto objects = std::get<std::vector<std::unique_ptr<service::storage::base::IObject>>>(std::move(value.second.second));
            SPDLOG_DEBUG("key: {}, size: {}", value.first, objects.size());

            for(auto&& obj : objects) {
              auto subvalues = obj->GetObjectValues();

              for(auto& subvalue : subvalues.value()) {
                SPDLOG_DEBUG(" --- key: {}, value: {}", subvalue.first, std::get<std::string>(subvalue.second.second));
              }
            }
          } else {
            SPDLOG_DEBUG("key: {}, value: {}", value.first, std::get<std::string>(value.second.second));
          }

        }

      }

    }
  }

  // Update
  {
    auto mongo_object_filter = std::make_unique<service::storage::mongo::MongoObject>();
    mongo_object_filter->AddPair(
      std::string("task-id"),
      std::make_pair(service::storage::base::StorageDataTypes::eInt, std::string("12")));

    auto mongo_object_query = std::make_unique<service::storage::mongo::MongoObject>();
    mongo_object_query->AddPair(
      std::string("client-id"),
      std::make_pair(service::storage::base::StorageDataTypes::eInt, std::string("3")));

    client.Update(std::move(mongo_object_filter), std::move(mongo_object_query));
  }

  // // Delete
  // {
  //   auto mongo_object = std::make_unique<service::storage::mongo::MongoObject>();
  //   mongo_object->AddPair(
  //     std::string("user-id"),
  //     std::make_pair(service::storage::base::StorageDataTypes::eInt, std::string("77777")));
  //   client.Delete(std::move(mongo_object));
  // }

  return 0;
}
