#!/bin/bash


if [[ ! -d "Build" ]]
then
    mkdir Build
    mkdir Build/Bin
    mkdir Build/Lib
fi

if [[ $1 == "core" ]]
then

    cd Build/Bin

    g++ -c ../../FWCoreLibrary/*.cpp -fPIC -fvisibility=default

    cd ..

    g++ -shared Bin/*.o -o Lib/libfwcore.so

    sudo cp Lib/libfwcore.so /usr/lib/libfwcore.so

    sudo chown $USER:$USER /usr/lib/libfwcore.so

    g++ ../FWHandlers/*.cpp /usr/lib/libfwcore.so -lexpat -lconfig -pthread -o DiasFirewall

    cd ..
fi

if [[ $1 == "test" ]]
then
    cd Build

    g++ ../FWTests/main.cpp Lib/libfwcore.so -lexpat -lconfig -o unittest

    cd ..
fi

if [[ $1 == "clean" ]]
then
    rm -r Build/
fi
