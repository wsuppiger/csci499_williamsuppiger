# CSCI499 Robust Systems Design: Caw and Faz 
### Overview
*"In this project you are to build the software infrastructure for a new Function-as-a-Service (FaaS) platform we’ll call Faz, and on top of Faz build the infrastructure for a social network platform we’ll call Caw.  Faz is intended to provide a very basic FaaS platform like AWS Lambda or Google Cloud Functions, and Caw is intended to have the same basic functionality of Twitter."- Project Description*
### Setup VM and Project
Start up a Vagrant VM with Ubuntu 20.04.2 and run the following commands:

Host Computer to create VM:
```
vagrant init bento/ubuntu-20.04
vagrant up
vagrant ssh
```
Guest VM:
```
sudo apt-get update && sudo apt-get upgrade
sudo apt-get install build-essential
sudo apt-get install cmake
git clone https://github.com/wsuppiger/csci499_williamsuppiger.git
cd csci499_williamsuppiger
mkdir build && cd build
cmake ..
make
```

### Tests
To run the Key Value test cases: 

`./key_value_test`

To run the Caw test cases: 

`./caw_function_test`

### Run C++ Components
**To run the Key Value storage application:**

`./key_value_server`

Restore its data from a file and store its data to a file:

`./key_value_server --store <file>`

**To run the Faz (FaaS) application:**

`./faz_server`

**To run the Caw application:**

Hooks all functions in Caw:

`./caw --hookall`

Unhooks all functions in Caw:

`./caw --unhookall`

Registers the given username:

`./caw --registeruser <username>`

Creates a new caw with the given text:

`./caw --user <username> --caw <caw text>`

Indicates that the new caw is a reply to the given id:

`./caw --user <username> --caw <caw text> --reply <reply caw id>`

Starts following the given username:

`./caw --user <username> --follow <username>`

Reads the caw thread starting at the given id:

`./caw --user <username> --read <caw id>`

Gets the user’s profile of following and followers:

`./caw --user <username> --profile`

Allows a user to stream a specific hashtag:

`./caw --user <username> --stream <hashtag>`

Lists available flags:

`./caw --help`

### Run Go CLI Component
**To Setup Go build:**

*Make sure to follow all the steps in "Setup VM and Project" first.*

To install Go, follow the directions found [here](https://levelup.gitconnected.com/installing-go-on-ubuntu-b443a8f0eb55) to install Go 1.16.1.

In the project root:

```
export GO111MODULE=on  # Enable module mode
go get google.golang.org/protobuf/cmd/protoc-gen-go \
         google.golang.org/grpc/cmd/protoc-gen-go-grpc
sudo apt install -y protobuf-compiler

mkdir src/go_caw/faz && mkdir src/go_caw/caw
protoc --go_out=src/go_caw/faz --go_opt=paths=source_relative \
    --go-grpc_out=src/go_caw/faz --go-grpc_opt=paths=source_relative \
    protos/faz.proto
protoc --go_out=src/go_caw/caw --go_opt=paths=source_relative \
    --go-grpc_out=src/go_caw/caw --go-grpc_opt=paths=source_relative \
    protos/caw.proto
go mod download
go build ./src/go_caw
```
**To Run the Caw CLI tool:**

*Uses all the same flags as the C++ implementation*

`./go_caw --flags`
