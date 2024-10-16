#pragma once

#include <memory>

#include "StorageObject.hpp"

namespace service::storage::base {

class IStorageClient {
  public:
    virtual ~IStorageClient() = default;

    virtual StorageError Create(
      std::unique_ptr<IObject> object) = 0;
    virtual StorageError Read(
      std::unique_ptr<IObject> query,
      std::vector<std::unique_ptr<IObject>>& values) = 0;
    virtual StorageError Update(
      std::unique_ptr<IObject> filter,
      std::unique_ptr<IObject> object) = 0;
    virtual StorageError Delete(
      std::unique_ptr<IObject> filter) = 0;
};

} // service::storage::base
