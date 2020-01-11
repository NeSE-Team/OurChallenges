FROM ubuntu:18.04

RUN set -xe \
    && sed -i 's/archive.ubuntu.com/mirrors.aliyun.com/g' /etc/apt/sources.list \
    && sed -i 's/security.ubuntu.com/mirrors.aliyun.com/g' /etc/apt/sources.list \
    && apt-get update \ 
    && apt-get install -y netcat socat \
    && apt-get autoremove -y \
    && mkdir /app/ \
    && mkdir /app/assets/

WORKDIR /app/

COPY  ./backup/server /app
COPY ./backup/flag.txt /flag


RUN chmod +x /app/server \
    && chown www-data:www-data /flag \
    && chown www-data:www-data /app/assets \
    && chmod 0400 /flag 

USER www-data

CMD ["/app/server"]