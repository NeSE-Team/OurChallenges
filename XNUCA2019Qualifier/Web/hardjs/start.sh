#!/bin/bash

/usr/local/bin/supercronic -debug /tmp/crontab &

find /var/lib/mysql -type f -exec touch {} \; && service mysql start

while true
do
    pkill node
    cd /app &&  npm install && npm start & 
    sleep 3600
    echo "restart ...."
done