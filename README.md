# Cloud

Simple implementation of synchronized data storage

## General structure

![general structure](/docs/general_structure.jpg)

### Client

Handler of users data storage

### Server

Storage of all clients copied data. For each client server has independent substorage that synchronized with client. If client has files or folders that server substorage has not, server loading them. If client deleted some files or folders, server deleting them too.

## General structure of server or client

![general structure of server or client](/docs/general_structure_of_server_or_client.jpg)

### Server network module

* Handle many clients at same time

### Client network module

* Provide permanent connection with server

### Files handling module

* Handle with OS filesystem

### View module

* UI

### Client main logic module

* Answer on servers requests

### Server main logic module

* Asking clients about any changes of users data
* Provide ordered requesting of needed users data
