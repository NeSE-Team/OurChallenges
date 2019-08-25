FROM ubuntu:16.04
RUN sed -i 's/archive.ubuntu.com/mirrors.aliyun.com/g' /etc/apt/sources.list
RUN sed -i 's/security.ubuntu.com/mirrors.aliyun.com/g' /etc/apt/sources.list


RUN apt-get -y update && \
    apt-get -y install -y python python-pip libmysqlclient-dev&& \ 
    apt-get -y autoremove && \
    rm -rf /var/lib/apt/lists/*
RUN useradd -m -d /home/oauth -s /bin/bash oauth &&\ 
    echo 'oauth:thisispassword!@#' | chpasswd 
RUN mkdir /oauth/
COPY ./src/  /oauth/
RUN pip install --upgrade pip
RUN pip install -r /oauth/requirements.txt
RUN pip install uwsgi
RUN pip install flask
WORKDIR /oauth

EXPOSE 80