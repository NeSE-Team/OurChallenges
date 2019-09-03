FROM ubuntu:18.04
RUN sed -i "s/http:\/\/archive.ubuntu.com/http:\/\/mirrors.tuna.tsinghua.edu.cn/g" /etc/apt/sources.list && \
    apt-get update && apt-get -y dist-upgrade && \
    apt-get install -y lib32z1 xinetd libssl1.1 net-tools

RUN useradd -m ctf
WORKDIR /home/ctf

COPY flag009d9713831dca08e944ed901aede7f1.txt /home/ctf
COPY flag3.txt /home/ctf
COPY flag1.txt /home/ctf
COPY serve_dir /home/ctf
COPY work_dir /home/ctf/work_dir

WORKDIR /home/ctf
RUN chmod 555 ./work_dir

CMD ["./serve_dir","30009","./work_dir"]
EXPOSE 30009
