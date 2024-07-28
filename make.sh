#!/bin/bash

# Compile server files
g++ -o server.exe server.cpp chatserver.cpp -lws2_32
if [ $? -eq 0 ]; then
    echo "Compiled server files successfully."
else
    echo "Failed to compile server files."
fi

# Compile client files
g++ -o client.exe client.cpp chatclient.cpp -lws2_32
if [ $? -eq 0 ]; then
    echo "Compiled client files successfully."
else
    echo "Failed to compile client files."
fi
