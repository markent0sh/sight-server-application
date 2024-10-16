#pragma once

#include <memory>
#include <optional>

#include "StorageClient.hpp"
#include "StorageObject.hpp"

namespace service::storage::base {

class IObjectCreator {
  public:
    virtual ~IObjectCreator() = default;

    virtual std::optional<std::unique_ptr<IObject>> Create() = 0;
};

class IClientCreator {
  public:
    virtual ~IClientCreator() = default;

    virtual std::optional<std::shared_ptr<IStorageClient>> Create() = 0;
};

} // service::storage::base
