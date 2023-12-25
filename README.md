# sync-click

This project creates a client-server architecture with TCP sockets in which the server sends a message to all connected clients simultaneously, and these ones generate a left click event at OS level.

# Overview

*TODO*

# How to use it

*TODO*

# Prerequisites
Make sure you have all of the following on your machine:
### Server
- POSIX-certified / POSIX-compliant OS (see https://en.wikipedia.org/wiki/POSIX#POSIX-oriented_operating_systems)
- GNU Make (tested on 4.3)
- gcc (tested on 10.2.1)
### Client
- Microsoft Windows OS (tested on Windows 10)
- Strawberry Perl (needed to have make and gcc on Windows, tested on 5.32.1.1)

# Build

### Server
To build the server executable just type the following command on console:

	make server

### Client
To build the client executable just type the following command on console:

	make client
