#!/usr/bin/env bash

# Build the dev image if it does not exist already
if ! docker inspect strace-dev &> /dev/null ; then
    docker build -t strace-dev -f Dockerfile.dev .
fi

docker run --rm -v $PWD:/strace -w /strace strace-dev make
