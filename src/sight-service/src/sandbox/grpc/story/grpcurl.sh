# enum PollFrequency {
#   RARE = 0;
#   MEDIUM = 1;
#   FAST = 2;
#   VERY_FAST = 3;
#   QUANTUM = 4;
# }

grpcurl -vv \
  -emit-defaults \
  -proto src/sight-service/src/service/server/story/proto/story/v1/story.proto \
  -plaintext \
  -d '{"client": 104372576, "target": {"username": "k137sd", "frequency": 2}}' \
  localhost:33333 story.v1.StoryService/Subscribe

grpcurl -vv \
  -emit-defaults \
  -proto src/sight-service/src/service/server/story/proto/story/v1/story.proto \
  -plaintext \
  -d '{"client": 104372576, "username": "k137sd"}' \
  localhost:33333 story.v1.StoryService/Unsubscribe

grpcurl -vv \
  -emit-defaults \
  -proto src/sight-service/src/service/server/story/proto/story/v1/story.proto \
  -plaintext \
  -d '{"client": 0, "username": "k137sd"}' \
  localhost:33333 story.v1.StoryService/GetAvailableStories
