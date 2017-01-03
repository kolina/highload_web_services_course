FROM ubuntu:14.04

RUN apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv 0C49F3730359A14518585931BC711F9BA15703C6

RUN echo "deb [ arch=amd64 ] http://repo.mongodb.org/apt/ubuntu trusty/mongodb-org/3.4 multiverse" > /etc/apt/sources.list.d/mongodb-org-3.4.list

RUN apt-get update

RUN apt-get install -y mongodb-org

CMD ["mongod", "--replSet", "rs0"]
