FROM ubuntu:16.04

RUN sed -i 's/archive.ubuntu.com/mirrors.ustc.edu.cn/g' /etc/apt/sources.list
RUN sed -i 's/security.ubuntu.com/mirrors.ustc.edu.cn/g' /etc/apt/sources.list

ENV DEBIAN_FRONTEND noninteractive
ENV FLAG NeSE{mix_up1s-alittle^disgusting}
ENV SECRET_KEY b1a2dd530719b65a9eca62fa052e7b6b
ENV PGPASS 769bf285b23c49cd563d7d8750bf9696

RUN apt-get -y update && \
    apt-get -y install nginx libpq-dev python-software-properties software-properties-common && \
    apt-get -y install postgresql && \
    LC_ALL=C.UTF-8 add-apt-repository -y ppa:jonathonf/python-3.6 && \
    apt-get update && \
    apt-get -y install python3.6-dev python3-pip && \
    apt-get -y autoremove  && \
    rm -rf /var/lib/apt/lists/*

RUN rm -rf /usr/bin/python3
RUN ln -s /usr/bin/python3.6 /usr/bin/python3
RUN python3 -m pip install --upgrade pip

COPY pip3 /usr/bin/pip3
RUN chmod +x /usr/bin/pip3
COPY pip.conf /root/.pip/pip.conf
RUN pip3 install secret django psycopg2 uwsgi supervisor

COPY db.sql /tmp/db.sql
RUN set -x \
    && service postgresql start \
    && su - postgres -c "psql -c \"CREATE USER ezcrypto WITH PASSWORD '769bf285b23c49cd563d7d8750bf9696';\"" \
    && su - postgres -c "psql -c \"CREATE DATABASE ezcrypto OWNER ezcrypto;\"" \
    && su - postgres -c "psql -d ezcrypto -c \"GRANT SELECT,INSERT,UPDATE,DELETE ON ALL TABLES IN SCHEMA public TO ezcrypto;\"" \
    && su - postgres -c "psql -d ezcrypto < /tmp/db.sql" \
    && rm /tmp/db.sql

COPY supervisord.conf /ezcrypto/supervisord.conf
COPY uwsgi.ini /ezcrypto/uwsgi.ini
COPY app /ezcrypto/app
COPY ezcrypto.conf /etc/nginx/sites-available/ezcrypto.conf

RUN set -x \
    && ln -s /etc/nginx/sites-available/ezcrypto.conf /etc/nginx/sites-enabled/ezcrypto.conf \
    && rm /etc/nginx/sites-available/default /etc/nginx/sites-enabled/default

COPY start.sh /start.sh
RUN chmod +x /start.sh

CMD ["/start.sh"]
