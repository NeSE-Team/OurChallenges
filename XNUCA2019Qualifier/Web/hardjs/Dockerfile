FROM node:latest

ENV DEBIAN_FRONTEND noninteractive  
ENV FLAG flag{4cc931e80cc8f8eceb6a0ea2fb532d8a}
RUN sed -i 's/deb.debian.org/mirrors.aliyun.com/g' /etc/apt/sources.list
RUN sed -i 's/security.debian.org/mirrors.aliyun.com/g' /etc/apt/sources.list

# install mysql
RUN apt-get update && apt-get -y dist-upgrade &&  \ 
    apt-get install -y mysql-server mysql-client && \
    /etc/init.d/mysql start && \ 
    apt-get install -y chromium chromium-driver unzip wget curl python python-pip && \ 
    pip install -U selenium 

# install chrome headless
# COPY ./attachment/google-chrome-stable_current_amd64.deb /tmp/
# RUN  apt-get update --fix-missing  && \ 
#     apt install -y /tmp/google-chrome*.deb &&  \
#     rm /tmp/google-chrome-stable_current_amd64.deb

# COPY ./attachment/chromedriver_linux64.zip /tmp/chromedriver.zip 
# RUN unzip /tmp/chromedriver.zip chromedriver -d /usr/local/bin/

# 下载supercron 
# ENV SUPERCRONIC_URL=https://github.com/aptible/supercronic/releases/download/v0.1.5/supercronic-linux-amd64 \
#    SUPERCRONIC=supercronic-linux-amd64
# RUN wget -O /usr/local/bin/${SUPERCRONIC}  "$SUPERCRONIC_URL"  && \ 

COPY ./attachment/supercronic-linux-amd64 /usr/local/bin/supercronic-linux-amd64 
RUN  ln -s /usr/local/bin/supercronic-linux-amd64 /usr/local/bin/supercronic && chmod +x "/usr/local/bin/supercronic"

COPY ./attachment/init.sql  /tmp/
COPY ./attachment/crontab /tmp/
COPY ./attachment/robot.py /tmp/
COPY ./start.sh /start.sh

RUN chmod +x /start.sh

# set mysql
#设置数据库 
RUN set -x \ 
    && service mysql start \
    && mysql -e "update mysql.user set authentication_string=password('root'),plugin = 'mysql_native_password' where user='root';" \
    && mysql -e "FLUSH PRIVILEGES;" \
    && mysql  -e "CREATE DATABASE app  DEFAULT CHARACTER SET utf8;"  -uroot  -proot \ 
    && sed -i "s?__FLAG__?${FLAG}?g" /tmp/init.sql \
    &&  mysql -e "use app;source /tmp/init.sql;"  -uroot -proot \
    && rm /tmp/init.sql 

# set bot 
RUN touch /var/log/cron.log

COPY ./source /app 

# clear
RUN apt-get  -y purge unzip wget curl && \
    apt-get -y autoremove && \ 
    rm -rf /var/lib/apt/lists/* 

WORKDIR /app/

CMD ["/start.sh"]