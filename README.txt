CS 447 project 2

This project is a simple implementation of an email server.
There are a total of three progams that can be run which are the server, client_sender, and client_receiver.
The client_sender is used to send emails to the server over smtp.
The client_receiver is used to retrieve emails from the server over http.
All processes accept a configuration file as a runtime command.
The server configuration file should have the format
    SMTP_PORT=(port number)
    HTTP_PORT=(port number)
The client configuration file should have the format
    SERVER_IP=(ip address)
    SERVER_PORT=(port number)
Instructions: run the make command in the terminal to compile all of the programs.
    start the server process first, then start the client_sender process to send emails to the server and
    start the client_receiver process to retrieve emails from the server. The server allows multiple client_sender processes to send
    emails simultaneously (multithreaded). The server allows only one client_receiver process to retrieve emails at one time (not multithreaded).
