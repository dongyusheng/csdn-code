FROM ubuntu:16.04

MAINTAINER dongyusheng "1286550014@qq.com"

ENV REFRESHED_AT 2020-07-19

RUN apt-get -yqq update && apt-get -yqq install ruby ruby-dev build-essential redis-tools

RUN gem install --no-rdoc --no-ri sinatra json redis

RUN mkdir -p /opt/webapp

EXPOSE 4567

CMD [ "/opt/webapp/bin/webapp" ]
