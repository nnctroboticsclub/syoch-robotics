#!/bin/bash

git init

git submodule add git@github.com:nnctroboticsclub/syoch-robotics syoch-robotics

cp -v syoch-robotics/skelton/project/* .
cp -v syoch-robotics/.devcontainer .

# MOUNT=/mnt/st1 PROJECT_DIR=stm32-main SERIAL=066AFF495057717867162927 TAG=s1 LOG_TAG='SerialProxy (UART: 1)' make ns
# TAG=e PROJECT_DIR=esp32 make ne
