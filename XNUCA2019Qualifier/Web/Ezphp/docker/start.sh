#!/bin/bash 

chmod 755 index.php

rm /var/log/apache2/access.log
rm /var/log/apache2/error.log

service apache2 start

tail -f /dev/null