# Network dummy project

Simple structured C++ project that uses HTTP and Websockets to communicate between async server and async clients.

## Tech stack

* CMake
* C++20
* Boost Asio Beast
* NlohmannJson
* Easylogging (Not the best choice)
* GTests

Libs loads on stage of project configuring by CMake

## Server

* Console interface
* Starting number of threads that equals number of processors cores
* Server address sets by config file
* Has its own web-pages
* Can send broadcast messages by keyboard to all websockets clients
* Can receive all websockets clients messages
* All logs storing in file

## Client

* Console interface
* Can handle server falling and reconnecting
* Main thread works with user interaction, another - network interaction
* Connecting address sets by config file
* Can send messages by keyboard to websocket server
* Can receive websocket server messages
* All logs storing in file
