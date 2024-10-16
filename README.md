# SR - Sight Server

SR - is a back-end OSINT service aimed at the average social media user needs.
SR provides APIs for collecting information on the following social networks:

- Instagram
- TikTok (planned)
- X (planned)

## Features

* Instagram

  * Target stories observer
  * Target followers observer
  * Target following observer
  * Private profile scraper (in progress)
  * Close friends stories (in progress)
  * Target likes observer (planned)
  * Target comments observer (planned)

## Development

### Sight Service

#### Requirements

* C++20
* Python 3.6
* CMake 3.2
* Protobuf 3.15.0
* MongoDB (mongocxx & bsoncxx)
* gRPC
* spdlog
* googletest
* crp
* nlohmann

#### Manual Installation

#### deb

```
sudo apt install git build-essential g++ gcc libssl-dev autoconf automake libtool curl libcurlpp-dev make g++ unzip htop gnupg python3-pip -y
```

##### CMake 3.2

```
curl -OL https://github.com/Kitware/CMake/releases/download/v3.29.3/cmake-3.29.3.tar.gz
cd cmake-3.29.3
./bootstrap
make -j$(nproc)
sudo make install
```

##### mongocxx

```
curl -OL https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.10.1/mongo-cxx-driver-r3.10.1.tar.gz
tar -xzf mongo-cxx-driver-r3.10.1.tar.gz
cd mongo-cxx-driver-r3.10.1/build
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DMONGOCXX_OVERRIDE_DEFAULT_INSTALL_PREFIX=OFF
cmake --build .
sudo cmake --build . --target install
```

##### mongodb

```
curl -fsSL https://www.mongodb.org/static/pgp/server-7.0.asc | \
   sudo gpg -o /usr/share/keyrings/mongodb-server-7.0.gpg \
   --dearmor
echo "deb [ arch=amd64,arm64 signed-by=/usr/share/keyrings/mongodb-server-7.0.gpg ] https://repo.mongodb.org/apt/ubuntu jammy/mongodb-org/7.0 multiverse" | sudo tee /etc/apt/sources.list.d/mongodb-org-7.0.list
sudo apt update
sudo apt install -y mongodb-org
```

##### mongosh

```
wget -qO- https://www.mongodb.org/static/pgp/server-7.0.asc | sudo tee /etc/apt/trusted.gpg.d/server-7.0.asc
echo "deb [ arch=amd64,arm64 ] https://repo.mongodb.org/apt/ubuntu jammy/mongodb-org/7.0 multiverse" | sudo tee /etc/apt/sources.list.d/mongodb-org-7.0.list
sudo apt update
sudo apt install -y mongodb-mongosh
```

##### Protobuf

```
curl -OL https://github.com/protocolbuffers/protobuf/archive/refs/tags/v3.15.0.tar.gz
tar -xzf v3.15.0.tar.gz
cd protobuf-3.15.0/
./autogen.sh && ./configure && make
sudo make install
```

##### cpr

```
git clone https://github.com/libcpr/cpr.git
cd cpr && mkdir build && cd build
cmake .. -DCPR_USE_SYSTEM_CURL=ON
cmake --build . --parallel
sudo cmake --install .
```

#### Build

```
git clone git@github.com:markent0sh/sight-server.git
cd sight-server
mkdir build && cd build
cmake ..  -DLOG_LEVEL=DEBUG -DBUILD_SANDBOX=ON
make -j6
```

#### Configurable

* Follower Service
  * gRPC

    * Address
    * Port
  * Mongo

    * Address
    * Port
    * DB
    * Collections
* Story Service
  * gRPC

    * Address
    * Port
  * Mongo

    * Address
    * Port
    * DB
    * Collections

#### Run

* story-sight-service
* follower-sight-service

### Instagram Scraper App

#### Requirements

* python3+
* pip

#### Run

> pip install -r src/scraper/instagram/requirements.txt
>
> python3 src/scraper/instagram/app.py
>
> curl -X POST http://127.0.0.1:5000/login

#### Configurable

* Accounts in **src/scraper/instagram/accounts.json**
