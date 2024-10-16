#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/json.hpp>
#include <iostream>

#include "Logger.hpp"

class MongoDBCRUD {
  public:
    MongoDBCRUD(const std::string& uri, const std::string& dbName, const std::string& collectionName) {
      mongocxx::uri uri_obj{uri};
      client = mongocxx::client{uri_obj};
      db = client[dbName];
      collection = db[collectionName];
    }

    void createDocument(const bsoncxx::builder::basic::document& doc) {
      auto result = collection.insert_one(doc.view());
      if (result) {
        std::cout << "Inserted document with id: "
              << result->inserted_id().get_oid().value.to_string() << std::endl;
      }
    }

    void readDocuments(const bsoncxx::builder::basic::document& query) {
      auto cursor = collection.find(query.view());
      for (auto&& doc : cursor) {
        std::cout << bsoncxx::to_json(doc) << std::endl;
      }
    }

    void updateDocument(const bsoncxx::builder::basic::document& filter, const bsoncxx::builder::basic::document& update) {
      auto result = collection.update_many(filter.view(), update.view());
      if (result) {
        std::cout << "Matched " << result->matched_count() << " documents and modified "
              << result->modified_count() << " documents." << std::endl;
      }
    }

    void deleteDocument(const bsoncxx::builder::basic::document& filter) {
      auto result = collection.delete_many(filter.view());
      if (result) {
        std::cout << "Deleted " << result->deleted_count() << " documents." << std::endl;
      }
    }

  private:
    mongocxx::client client;
    mongocxx::database db;
    mongocxx::collection collection;
};


int main() {
  mongocxx::instance inst{}; // Initializes the MongoDB C++ Driver

  MongoDBCRUD crud("mongodb://localhost:27017", "testdb", "testcollection");

  // Create a new document
  auto createDoc = bsoncxx::builder::basic::document{};
  createDoc.append(bsoncxx::builder::basic::kvp("name", "John Doe"), bsoncxx::builder::basic::kvp("age", 32));

  crud.createDocument(createDoc);

  // Query documents with name "John Doe"
  auto readQuery = bsoncxx::builder::basic::document{};
  readQuery.append(bsoncxx::builder::basic::kvp("name", "John Doe"));

  crud.readDocuments(readQuery);

  // Update documents: set age to 31 where name is "John Doe"
  auto updateFilter = bsoncxx::builder::basic::document{};
  updateFilter.append(bsoncxx::builder::basic::kvp("name", "John Doe"));

  auto updateValue = bsoncxx::builder::basic::document{};
  updateValue.append(bsoncxx::builder::basic::kvp("age", 31));

  auto updateDoc = bsoncxx::builder::basic::document{};
  updateDoc.append(bsoncxx::builder::basic::kvp("$set", updateValue));

  crud.updateDocument(updateFilter, updateDoc);

  // Delete documents where age is 31
  auto deleteFilter = bsoncxx::builder::basic::document{};
  deleteFilter.append(bsoncxx::builder::basic::kvp("age", 31));

  crud.deleteDocument(deleteFilter);

  return 0;
}
