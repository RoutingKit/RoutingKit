FROM ubuntu:18.04
RUN apt-get update
RUN apt-get -qq -y install zlib1g-dev
