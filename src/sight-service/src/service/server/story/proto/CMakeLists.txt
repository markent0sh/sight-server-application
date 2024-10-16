cmake_path(
  SET
    story_proto
      ${CMAKE_CURRENT_SOURCE_DIR}/story/v1/story.proto
)

cmake_path(
  SET
    story_grpcxx_header
      ${CMAKE_CURRENT_BINARY_DIR}/story/v1/story.grpcxx.pb.h
)

cmake_path(
  SET
    story_header
      ${CMAKE_CURRENT_BINARY_DIR}/story/v1/story.pb.h
)

cmake_path(
  SET
    story_source
      ${CMAKE_CURRENT_BINARY_DIR}/story/v1/story.pb.cc
)

set(
  protos
	  ${story_proto}
)

set(
  headers
	  ${story_header}
      ${story_grpcxx_header}
)

set(
  sources
	  ${story_source}
)

add_custom_command(
	OUTPUT  ${headers} ${sources}
	DEPENDS ${protos}
	COMMAND ${Protobuf_PROTOC_EXECUTABLE}
	ARGS
		--proto_path=${CMAKE_CURRENT_SOURCE_DIR}
		--proto_path=${Protobuf_INCLUDE_DIR}
		--cpp_out=${CMAKE_CURRENT_BINARY_DIR}
		--grpcxx_out=${CMAKE_CURRENT_BINARY_DIR}
		--plugin=protoc-gen-grpcxx=$<TARGET_FILE:libgrpcxx::protoc-gen>
		${protos}
)

add_library(
  story-proto
    ${sources}
)

target_include_directories(
  story-proto
	  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

target_link_libraries(
  story-proto
	  protobuf::libprotobuf
)
