FROM ubuntu:16.04

MAINTAINER dongyusheng "1286550014@qq.com"

ENV REFRESHED_AT 2020-07-19

RUN apt-get -yqq update && apt-get -yqq install redis-server redis-tools

EXPOSE 6379

ENTRYPOINT ["/usr/bin/redis-server" ]
CMD []
