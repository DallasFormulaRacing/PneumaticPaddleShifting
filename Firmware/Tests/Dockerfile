# Fetch the Ubuntu image as a base
FROM ubuntu:22.04

RUN \
    apt-get update && \
    apt-get install -y python3 && \
    apt-get install -y build-essential && \
    apt-get install -y ninja-build && \
    apt-get install -y cmake