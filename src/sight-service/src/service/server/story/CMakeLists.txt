add_library(
  story-server-public
    INTERFACE
      ${CMAKE_CURRENT_SOURCE_DIR}/public/StoryServer.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/public/StoryTypes.hpp
)

target_include_directories(
  story-server-public
    INTERFACE
      ${CMAKE_CURRENT_SOURCE_DIR}/public/
)

add_library(
  story-server-private
    STATIC
      ${CMAKE_CURRENT_SOURCE_DIR}/private/StoryServerImpl.cpp
)

target_include_directories(
  story-server-private
    PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/public
)

target_link_libraries(
  story-server-private
    PUBLIC
		  story-proto
      sight-common-public
		  libgrpcxx::grpcxx
      spdlog::spdlog
)

add_subdirectory(proto)
