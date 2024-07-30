# Network Communication Software

A C++ based client-server application for group chat communication.

## Features

- Client-server architecture
- Group-based chat rooms
- User authentication (login, register, update, delete account)
- Real-time messaging

## Project Structure

- `Server/`: Contains server-side code
  - `server.cpp`: Main server application
- `Client/`: Contains client-side code
  - `client.cpp`: Main client application
- `database.txt`: Stores user credentials

## Requirements

- Windows OS
- C++ compiler
- Winsock2 library

## Usage

- Can use Vscode2019 to run and test the client and server

### Server

1. Compile and run `server.cpp`
2. The server will start listening for connections on port 5019

### Client

1. Compile and run `client.cpp`
2. Choose from the following options:
   - Login
   - Register
   - Update account
   - Delete account
3. After successful authentication, join a chat group and start messaging

## How It Works

- The server accepts multiple client connections
- Clients can join specific chat groups
- Messages are broadcast to all clients in the same group
- User credentials are stored and managed in `database.txt`
