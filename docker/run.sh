#!/bin/bash

SERVER_IP=$1
SERVER_PORT=$2

export SERVER_IP=$1 && export SERVER_PORT=$2 && ~/micro-service/slapi_rest

