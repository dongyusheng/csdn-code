FROM ubuntu:16.04
LABEL maintainer="https://blog.csdn.net/qq_41453285/"
ENV REFRESHED_AT 2020-07-27

RUN apt-get -qq update
RUN apt-get -qq install wget

VOLUME [ "/var/lib/tomcat8/webapps/" ]
WORKDIR /var/lib/tomcat8/webapps/

ENTRYPOINT [ "wget" ]
CMD [ "--help" ]

