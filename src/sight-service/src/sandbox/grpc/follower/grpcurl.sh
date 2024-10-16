# enum PollFrequency {
#   RARE = 0;
#   MEDIUM = 1;
#   FAST = 2;
#   VERY_FAST = 3;
#   QUANTUM = 4;
# }

grpcurl -vv \
  -emit-defaults \
  -proto src/sight-service/src/service/server/follower/proto/follower/v1/follower.proto \
  -plaintext \
  -d '{"client": 0, "target": {"username": "k137sd", "followers": true, "following": true, "frequency": 4}}' \
  localhost:44444 follower.v1.FollowerService/Subscribe

grpcurl -vv \
  -emit-defaults \
  -proto src/sight-service/src/service/server/follower/proto/follower/v1/follower.proto \
  -plaintext \
  -d '{"client": 0, "username": "k137sd"}' \
  localhost:44444 follower.v1.FollowerService/Unsubscribe

grpcurl -vv \
  -emit-defaults \
  -proto src/sight-service/src/service/server/follower/proto/follower/v1/follower.proto \
  -plaintext \
  -d '{"client": 1, "username": "k137sd", "from": {"date": "09.04.2024", "time": "21:44"}, "to": {"date": "01.08.2024", "time": "20:00"}}' \
  localhost:44444 follower.v1.FollowerService/GetFollowers

grpcurl -vv \
  -emit-defaults \
  -proto src/sight-service/src/service/server/follower/proto/follower/v1/follower.proto \
  -plaintext \
  -d '{"client": 0, "username": "k137sd", "from": {"date": "10.04.2024", "time": "01:56"}, "to": {"date": "01.08.2024", "time": "20:00"}}' \
  localhost:44444 follower.v1.FollowerService/GetFollowing

grpcurl -vv \
  -emit-defaults \
  -proto src/sight-service/src/service/server/follower/proto/follower/v1/follower.proto  \
  -plaintext \
  -d '{"client": 0}' \
  localhost:44444 follower.v1.FollowerService/GetClientTargets
