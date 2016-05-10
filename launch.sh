#!/bin/bash

clear
clear

export NULL=/dev/null
mkdir -p build
pushd build >& $NULL
    make distclean
    qmake ..
    make
popd >& $NULL

pushd build >& $NULL
    gnome-terminal -x bash -c "./external 6 600; echo done; read -p \"\""
    sleep 1s
    gnome-terminal -x bash -c "./external 5 500; echo done; read -p \"\""
    sleep 1s
    gnome-terminal -x bash -c "./external 4 400; echo done; read -p \"\""
    sleep 1s
    gnome-terminal -x bash -c "./external 3 300; echo done; read -p \"\""
    sleep 1s
    gnome-terminal -x bash -c "./external 2 200; echo done; read -p \"\""
    sleep 1s
    gnome-terminal -x bash -c "./external 1 100; echo done; read -p \"\""
    sleep 1s
    gnome-terminal -x bash -c "./external 0   0; echo done; read -p \"\""
popd >& $NULL
