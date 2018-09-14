#!/bin/bash

scp ./server alvis@$IP_ECS:/home/alvis/
ssh alvis@$IP_ECS "mv /home/alvis/server /home/alvis/MYTQ/server"
ssh alvis@$IP_ECS  "screen -dmS 'Server' server"
ssh alvis@$IP_ECS
