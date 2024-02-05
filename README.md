# Simple-TCP-Server-Client-CPP-Example

This test involves creating two programs – a server and a client program where the client sends a sequence of commands to create, read,
update, and delete data, and interprets the responses from the server, and the server properly handles the responses, using standard TCP
socket connections.
The format of the protocol will be a hypothetical "simple data transfer protocol" (i.e. SDTP), as a sort of HTTP-like protocol from an alternative
universe, supporting a simple key-value store.
The sdtp-server should bind and listen to port 2020 and be able to handle simultaneous connections from multiple clients.
The sdtp-client should default to connecting to localhost, port 2020, but should support command line parameters to connect to a
different host.

# Usage

To use on Linux:

Open terminal and copy-paste following lines:

```
cd server
make
make check
```
Open another terminal and copy-paste following lines:


```
cd client
make
./exe/mg_client [<ip-address> <port> <cmd> <key> [ < file ]
example: ./exe/mg_client 127.0.0.1 2020 create "MG" < datafile1
```
 