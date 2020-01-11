#!/bin/bash

set -xe 

#  &&

backup=$(pwd)/backup
binary=${backup}/server
flag=${backup}/flag.txt


buildServer(){
    docker build -t builder  -f ./go/Dockerfile.build    ./
    docker run -d --rm  -v ${backup}:/app/ builder /bin/cp /tmp/server /app/server
}


dockerStart(){
    docker-compose down
    docker-compose build
    docker-compose up -d
}

if [[ ! -f "${flag}" ]];then

    echo "flag.txt is not exist."
    echo "flag{this_is_a_fake_flag}" > ${flag}
fi

if [[ ! -f "${binary}" ]];then

    echo "server binary is not exist." 
    echo "build server from dockerfile "
    buildServer
fi

dockerStart
