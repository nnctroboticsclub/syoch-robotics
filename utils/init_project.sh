#!/bin/bash

git init

git submodule add git@github.com:nnctroboticsclub/syoch-robotics syoch-robotics

cp -v syoch-robotics/skelton/project/* .
