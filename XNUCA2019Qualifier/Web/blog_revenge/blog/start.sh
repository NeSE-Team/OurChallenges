#!/bin/bash 

sed -i "s?__HOST__?${HOST}?g" /var/www/protected/config.php
sed -i "s?__OAUTHSERVER__?${OAUTHSERVER}?g" /var/www/protected/config.php
sed -i "s?__FLAG__?${FLAG}?g" /var/www/protected/config.php

php-fpm7.2
service nginx start

/usr/local/bin/supercronic -debug /tmp/crontab
tail -f /dev/null