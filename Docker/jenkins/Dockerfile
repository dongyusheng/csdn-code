FROM jenkins/jenkins:lts
MAINTAINER https://blog.csdn.net/qq_41453285
ENV REFRESHED_AT 2020-07-27

USER root
RUN apt-get -qq update && apt-get install -qq sudo
RUN echo "jenkins ALL=NOPASSWD: ALL" >> /etc/sudoers
RUN wget http://get.docker.com/builds/Linux/x86_64/docker-latest.tgz
RUN tar -xvzf docker-latest.tgz
RUN mv docker/* /usr/bin/

USER jenkins
RUN /usr/local/bin/install-plugins.sh junit git git-client ssh-slaves greenballs chucknorris ws-cleanup
