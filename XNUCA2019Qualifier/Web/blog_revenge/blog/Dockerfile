FROM ubuntu:18.04
ENV DEBIAN_FRONTEND noninteractive

RUN sed -i 's/archive.ubuntu.com/mirrors.aliyun.com/g' /etc/apt/sources.list
RUN sed -i 's/security.ubuntu.com/mirrors.aliyun.com/g' /etc/apt/sources.list


RUN apt-get -y update && \
    apt-get -y install nginx php php-fpm php-mysql chromium-browser chromium-chromedriver unzip wget curl python python-pip&& \
    apt-get -y install netcat && \
    apt-get -y  autoremove  && \
    rm -rf /var/lib/apt/lists/*  && \
    pip install selenium 
    
COPY ./start.sh /start.sh
COPY ./php /etc/nginx/sites-enabled/default
RUN chmod +x /start.sh
# RUN rm /var/www/html/index.html
COPY ./src/protected /var/www/protected
COPY ./src/web /var/www/html
RUN chmod a+w /var/www/protected/tmp


# 下载supercron 
ENV SUPERCRONIC_URL=https://github.com/aptible/supercronic/releases/download/v0.1.5/supercronic-linux-amd64 \
   SUPERCRONIC=supercronic-linux-amd64 


RUN curl -fsSLO "$SUPERCRONIC_URL" \
 && chmod +x "$SUPERCRONIC" \
 && mv "$SUPERCRONIC" "/usr/local/bin/${SUPERCRONIC}" \
 && ln -s "/usr/local/bin/${SUPERCRONIC}" /usr/local/bin/supercronic

# COPY ./supercronic-linux-amd64 /usr/local/bin/supercronic-linux-amd64
# RUN  ln -s /usr/local/bin/supercronic-linux-amd64 /usr/local/bin/supercronic && chmod +x /usr/local/bin/supercronic

COPY crontab /tmp/
COPY robot.py /tmp/
RUN touch /var/log/cron.log && \ 
   apt-get  -y purge unzip wget curl && \
   apt-get -y autoremove  && \
   mkdir /run/php/

EXPOSE 80
CMD ["/start.sh"]
