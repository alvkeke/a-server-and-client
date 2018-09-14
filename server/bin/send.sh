#!/bin/bash

scp ./server alvis@$IP_ECS:/home/alvis/
ssh alvis@$IP_ECS "mv /home/alvis/server /home/alvis/MYTQ/server"
