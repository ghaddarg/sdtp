##Intro

This test involves creating two programs – a server and a client program where the client sends a sequence of commands to create, read,
update, and delete data, and interprets the responses from the server, and the server properly handles the responses, using standard TCP
socket connections.
The format of the protocol will be a hypothetical "simple data transfer protocol" (i.e. SDTP), as a sort of HTTP-like protocol from an alternative
universe, supporting a simple key-value store.
The sdtp-server should bind and listen to port 2020 and be able to handle simultaneous connections from multiple clients.
The sdtp-client should default to connecting to localhost, port 2020, but should support command line parameters to connect to a
different host.

##Usage
<.exe> [ <ip> <port> ] <cmd> <key> [ < file ]