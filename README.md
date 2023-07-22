
## CI-FTPD - Aaron Urrea
 - Course: COMP-429 Computer Networks
 - Professor: Kevin Scrivnor

## Table of Contents

- [Overview](#overview)
- [Reading](#reading)
- [Implementation](#implementation)
- [Testing](#testing)
- [Debugging](#debugging)
- [Requirements](#requirements)
- [Submit](#submit)
- [Appendix A: Example UDP Echo Server](#appendix-a-example-udp-echo-server)
- [Appendix B: Survive a Simple Hack](#appendix-b-survive-a-simple-hack)
- [C Header File (tftp)](#c-header-file-tftp)
- [C Source File (ciftpd)](#c-source-file-ciftpd)
- [Version](#version)

## Overview

For program 2, this project involves writing a simple TFTP server called `ciftpd` that can handle read requests and write requests from a standard Linux TFTP client. The server implements most of RFC 1350 and aims to provide a reliable file transfer mechanism.

## Reading

- [RFC 1350: Trivial File Transfer Protocol (revision 2)](https://tools.ietf.org/html/rfc1350)
- Chapter 8.* from the Stevens book
- Chapter 22.* from the Stevens book

## Implementation

### qanda

The `qanda` section lists important questions to consider before writing the code, including general knowledge about TFTP, C coding, and network programming.

### Example Struct

```c
// DATA packet
typedef struct {
    uint16_t opcode;
    uint16_t block;
    char data[BLKSIZE];
} DP;
```

### RFC 1350

This project implements much of RFC 1350, with a focus on handling read requests (RRQ) and write requests (WRQ) in netascii mode.

## Testing

Instructions for testing the `ciftpd` server using a Linux TFTP client are provided. Wireshark and gdb are suggested for debugging network traffic and finding bugs.

## Requirements

This section outlines the specific requirements for the project and the expected milestones for submission.

## Submit

Submission details and deadlines for the project are listed here.

## Appendix A: Example UDP Echo Server

A simple example of an UDP Echo Server (`echosrv.c`) is provided as a reference for understanding networking basics.

## Appendix B: Survive a Simple Hack

A challenge to test the robustness of your TFTP server against a provided hack.

## C Header File (tftp)

This section presents the content of the C header file (`tftp.h`) containing function prototypes and macros for the `ciftpd` implementation.

## C Source File (ciftpd)

This section presents the content of the C source file (`ciftpd.c`) that implements the `ciftpd` TFTP server, including the main function and various helper functions.

## Version

The current version of the project is 1.3.23s, last updated on 2023-03-06 15:12:37 -0800.
