CREATE DATABASE `oauth` DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

CREATE DATABASE `blog` DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;
use `blog`;

drop table if exists  `user`; 
create table `user` (
    `id` int(32) primary key auto_increment,
    `username` varchar(40) not null,
    `password` varchar(40) not null,
    `email` varchar(40) ,
    `uuid` varchar(20),
    `bind` boolean DEFAULT FALSE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ;

insert into `user` (`id`,`username`,`password`,`email`,`uuid`,`bind`) values ('1','admin',(select md5('LUUpoBCc7nM1HAJo')),'admin@qq.com','yAHZ0TV2hV6nBm5s4Yd6',TRUE);

drop table if exists `bug`;

create table `bug` (
    `id` int(32) primary key  auto_increment,
    `userid` int(32) not null,
    `bugurl` text not null,
    `read` boolean DEFAULT FALSE
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

drop table if exists `note`;
create table `note`(
    `id` int(32) primary key auto_increment,
    `userid` int(32) not null,
    `title` text not null,
    `content` text not null
)
