#!/bin/bash

service postgresql start

supervisord -c /ezcrypto/supervisord.conf

service nginx start

tail -f /dev/null