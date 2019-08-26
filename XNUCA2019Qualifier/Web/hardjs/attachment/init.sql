drop table if exists  `user`;

create table `user` (
    `id` int(32) primary key auto_increment,
    `username` varchar(40) not null,
    `password` varchar(40) not null
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ;

insert into `user` (`id`,`username`,`password`) values ('1','admin','__FLAG__');
drop table if exists `html`;

create table `html` (
    `id` int(32) primary key  auto_increment,
    `userid` int(32) not null,
    `dom` text not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8 ;