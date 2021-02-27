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
git clone https://github.com/williamsuppiger/csci499_williamsuppiger.git
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

### Run Components
**To run the Key Value storage application: **

`./key_value_server`

**To run the Faz (FaaS) application:**

`./faz_server`

**To run the Caw application:**

Hooks all functions in Caw:

`./caw --hookall`

Unhooks all functions in Caw:

`./caw --unhookall`

*Registers the given username:* 

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

Lists available flags:

`./caw --help`
