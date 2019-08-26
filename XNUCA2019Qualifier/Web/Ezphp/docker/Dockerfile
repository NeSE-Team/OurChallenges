FROM php:7.0-apache

COPY 000-default.conf /etc/apache2/sites-enabled/000-default.conf

COPY start.sh /start.sh

RUN chmod +x /start.sh

COPY php.ini /usr/local/etc/php/php.ini

COPY flag /flag

COPY index.php /var/www/html/index.php

RUN rm /var/www/html/index.html

CMD ["/start.sh"]