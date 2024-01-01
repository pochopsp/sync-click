# sync-click

This project creates a client-server architecture with TCP sockets in which the server sends a message to all connected clients simultaneously, and these ones generate a left click event at OS level.

# Overview

Both server and client are implemented in C. The server is made for POSIX compliant systems, while the client is made for Microsoft Windows since it uses its libs for sockets and hardware inputs.

Network communication is made through TCP sockets using a simple communication protocol defined for this app.
Basically, the server sends the message *CLK_CMD* and the clients generate a left click event using OS native libs.

To have the smallest delay as possible in a simultaneous click among the connected clients, I implemented the following logic:  
every client has a specific RTT, the server keeps track of each RTT and when the number of desired clients is reached, it sends the max RTT to the clients.
Then, when the server sends the message *CLK_CMD*, each client will wait for a time equal to the difference between max RTT and its own RTT before executing the click event.  

#### Example
Let's say we have two clients: *cl_A* with a RTT of 3 millis and *cl_M* with a RTT of 10 millis.  
When the server sends the message *CLK_CMD*, the following will happen:
	
 	19:26:00.400 - server sends CLK_CMD
 	19:26:00.430 - cl_A receives CLK_CMD and waits (MAX_RTT - MY_RTT) = (10 millis -  3 millis) = 7 millis
  	19:26:00:440 - cl_M receives CLK_CMD and waits (MAX_RTT - MY_RTT) = (10 millis - 10 millis) = 0 millis
   
This way, *client_A* awaits 7 millis and *client_M* awaits 0 millis, so that they **both** generate a click event at time **19:26:00:440**, with the smallest possible delay.

# Prerequisites
Make sure you have all of the following on your machine:
### Server
- POSIX-certified / POSIX-compliant OS (see [wikipedia POSIX operating systems](https://en.wikipedia.org/wiki/POSIX#POSIX-oriented_operating_systems))
- GNU Make (tested on 4.3)
- gcc (tested on 10.2.1)
### Client
- Microsoft Windows OS (tested on Windows 10)
- Strawberry Perl (to have make, gcc and POSIX functs such as getopt on Windows, tested on 5.32.1.1)

# Build

### Server
To build the server executable just type the following command on console:

	make server

### Client
To build the client executable just type the following command on console:

	gmake client

# How to use it

### Server
To start the server executable, you must give it the mandatory argument *<interface_name>* to specify on which network interface the socket will be listening.  
For example, to have it running on eth0 interface and TCP port 45230 start it like this:
	
 	./server eth0 -p 45230

It will give you some information on connected clients and, when they'll be all connected, you can start giving commands from command line. Available commands are **clic** to send click events to the clients and **cilc** to exit.  
Use ```./server -h ``` to have full information about options and operands.

### Client
To start the client executable, you must give it with the mandatory argument *<ip_address>* to specify server address.  
For example, to have it connected with server at ip 123.231.123.231 and TCP port 45230 start it like this:
	
 	./client 123.231.123.231 -p 45230

It will give you some information about RTT for current connection and max RTT, along with logs about received click messages.  
Use ```./client -h ``` to have full information about options and operands.

# The story behind it
My girlfriend and I use to watch old tv shows and we don't live together, so we talk over the phone and sometimes we use online streaming platforms, some other times we have offline videos playing with VLC.    
Each time we want to watch an episode, we count from 1 to 3 and then click play trying to be as precise as possible. This ain't the right way, and you can imagine that most of the times we aren't synced.

So I came up with the idea of having a synced click over network and started googling for an app that does it, and guess what? I wasn't able to find one, so I decided to make it in my spare time. And there is how this project was born (thanks darling for inspiring me).
