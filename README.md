## CI-FTPD - Aaron Urrea
 - Course: COMP-429 Computer Networks
 - Professor: Kevin Scrivnor

## Table of Contents

- [Overview](#overview)
- [Reading](#reading)
  - [Other Resources](#other-resources)
- [Implementation](#implementation)
  - [qanda](#qanda)
  - [Example Struct](#example-struct)
  - [RFC 1350](#rfc-1350)
- [Testing](#testing)
- [Debugging](#debugging)
  - [Wireshark](#wireshark)
  - [gdb](#gdb)

## Overview

For program 2, we will write a simple TFTP server that can handle both read requests and write requests from a standard Linux tftp client. We will be implementing most of RFC 1350, hence the name CIFTP rather than TFTP (the D stands for daemon, typical of server implementations: sshd, tftpd, etc…​).

## Reading

- [RFC 1350: Trivial File Transfer Protocol (revision 2)](https://tools.ietf.org/html/rfc1350)
- Chapter 8.* from the Stevens book
- Chapter 22.* from the Stevens book
- Rich Stevens took great care in his typesetting, the online version of the book does no justice for the hard work he put in to make sure every page looked perfect. You can read it about [here](https://users.cs.duke.edu/~reilly/mdc/books1.htm).

### Other Resources

- [21st Century C](https://nostarch.com/21stcenturyc)
- [The Linux Programming Interface](http://man7.org/tlpi/)
  - Socket chapters, File I/O

## Implementation

### Example Struct

```c
// DATA packet
typedef struct {
	uint16_t opcode;
	uint16_t block;
	char data[BLKSIZE];
} DP;
```
This will guarantee that we use 2 bytes each for the opcode and block.

## RFC 1350

We are implementing much of RFC 1350, but not all of it.

- The server must listen and wait for either a RRQ or WRQ.
  - Only need to implement netascii mode.
- The TID is the port number of the socket.
- Errors 1, 4, and 6 must be implemented.
- Timeouts are not required at this point.
- Retransmit the DATA packet if ACK is incorrect during an RRQ.
- Retransmit the last ACK if DATA arrives out of order.

## Testing

Be careful not to overwrite the files you are serving with the files from the client! For example, doing a read request in the same folder as where ciftpd hosts files.

To test your program, you want to make sure it works with a Linux tftp client.

```
Install the tftp client:

sudo apt install tftp
```

Open two terminals:
- Run ciftpd in one (as root, we need to be root to bind to ports < 1024).
- Run `tftp 127.0.0.1` in another, then type "get [filename]" or "put [filename]".

```
$ tftp 127.0.0.1
tftp> get test.txt
Received 24874 bytes in 0.0 seconds
```

Check the contents of the files you have transferred in both locations.

## Debugging

### Wireshark

To debug network traffic, launch Wireshark from the Linux system and monitor the l0 interface (loopback). A simple udp and !dns filter will suffice; there isn’t much traffic here to begin with.

Here are some examples from a working server:

prog02-tftp-rrq.pcapng
prog02-tftp-wrq.pcapng
prog02-tftp-error.pcapng

## Debugging

### gdb

Debugging with gdb has a small challenge; we are forking. Usually, gdb follows the parent process after a fork, but we can change it by using `set follow-fork-mode child` or by attaching to the child PID after execution has started with `gdb -p [PID]`.

