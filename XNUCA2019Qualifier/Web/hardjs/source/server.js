const fs = require('fs')
const express = require('express')
const bodyParser = require('body-parser')
const lodash = require('lodash')
const session = require('express-session')
const randomize = require('randomatic')
const mysql = require('mysql')
const mysqlConfig = require("./config/mysql")
const ejs = require('ejs')

const pool = mysql.createPool(mysqlConfig)


const app = express()

app.use(bodyParser.urlencoded({extended: true})).use(bodyParser.json())
app.use('/static', express.static('static'))
app.use(session({
    name: 'session',
    secret: randomize('aA0', 16),
    resave: false,
    saveUninitialized: false
}))

function setHeader(req,res,next){
    res.set({
        "X-DNS-Prefetch-Control":"off",
        "X-Frame-Options": "SAMEORIGIN",
        "X-Download-Options": "noopen",
        "X-Content-Type-Options": "nosniff"
    });
    next();
}

app.use(setHeader);

app.set('json escape',true)
app.set('views', './views')
app.set('view engine', 'ejs')


function auth(req,res,next){
    // var session = req.session;
    if(!req.session.login || !req.session.userid ){
        res.redirect(302,"/login");
    } else{
        next();
    }    
}


let query = function( sql, values ) {
    return new Promise(( resolve, reject ) => {
      pool.getConnection(function(err, connection) {
        if (err) {
          reject( err )
        } else {
          connection.query(sql, values, ( err, rows) => {
  
            if ( err ) {
              reject( err )
            } else {
              resolve( rows )
            }
            connection.release()
          })
        }
      })
    })
}


app.get("/",auth,function(req,res,next){
    res.render('index');
})

app.get("/sandbox",auth,function(req,res,next){
    res.render("sandbox");
})

app.all('/login',async function(req,res,next){

    if( req.method == 'POST' ){
        if(req.body.username && req.body.password){
            
            var username = req.body.username;
            var password = req.body.password;

            var sql = "select id from `user` where username=? and password=?";

            let dataList = await query( sql,[username,password])

            if(dataList.length == 0){
                res.send("<script>  alert('用户名或密码错误.'); location.replace('/login'); </script>");
                return ;
            }
            console.log(dataList);
            req.session.userid = dataList[0].id ;
            req.session.login = true ;
            res.redirect(302,"/");
            return ;

        }
    }

    res.render('login_register',{
        title:" storeHtml | logins ",
        buttonHintF:"登 录",
        buttonHintS:"没有账号?",
        hint:"登录",
        next:"/register"
    });
});




app.all('/register',async function(req,res,next){

    console.log(req.body);

    if( req.method == 'POST' ){

        if(req.body.username && req.body.password){

            var username = req.body.username;
            var password = req.body.password;
                        
            var sql = "select id from `user` where username=?";

            let dataList = await query( sql,[username])

            if(dataList.length>0){
                res.send("<script>alert('用户名重复'); location.replace('/register');</script>");
                return ;
            }
            
            var sql = "insert into `user` (`username`,`password`) values (?,?) ";
            
            let result = await query( sql,[username,password]);
            console.log(result);

            if(result.affectedRows>0){
                res.send("<script>alert('注册成功');location.replace('/login');</script>")
                return; 
            }else{
                res.send("<script>alert('注册失败');</script>");
                return; 
            }
        }
    }

    res.render('login_register',{
        title:" storeHtml | register ",
        buttonHintF:"注 册",
        buttonHintS:"已有账号?",
        hint:"注册",
        next:"/login"
    });
});

app.get("/get",auth,async function(req,res,next){

    var userid = req.session.userid ; 
    var sql = "select count(*) count from `html` where userid= ?"
    // var sql = "select `dom` from  `html` where userid=? ";
    var dataList = await query(sql,[userid]);

    if(dataList[0].count == 0 ){
        res.json({})

    }else if(dataList[0].count > 5) { // if len > 5 , merge all and update mysql
        
        console.log("Merge the recorder in the database."); 

        var sql = "select `id`,`dom` from  `html` where userid=? ";
        var raws = await query(sql,[userid]);
        var doms = {}
        var ret = new Array(); 

        for(var i=0;i<raws.length ;i++){
            lodash.defaultsDeep(doms,JSON.parse( raws[i].dom ));

            var sql = "delete from `html` where id = ?";
            var result = await query(sql,raws[i].id);
        }
        var sql = "insert into `html` (`userid`,`dom`) values (?,?) ";
        var result = await query(sql,[userid, JSON.stringify(doms) ]);

        if(result.affectedRows > 0){
            ret.push(doms);
            res.json(ret);
        }else{
            res.json([{}]);
        }

    }else {

        console.log("Return recorder is less than 5,so return it without merge.");
        var sql = "select `dom` from  `html` where userid=? ";
        var raws = await query(sql,[userid]);
        var ret = new Array();

        for( var i =0 ;i< raws.length ; i++){
            ret.push(JSON.parse( raws[i].dom ));
        }

        console.log(ret);
        res.json(ret);
    }

});

app.post("/add",auth,async function(req,res,next){

    if(req.body.type && req.body.content){

        var newContent = {}
        var userid = req.session.userid;

        newContent[req.body.type] = [ req.body.content ]

        console.log("newContent:",newContent);

        var sql = "insert into `html` (`userid`,`dom`) values (?,?) ";
        var result = await query(sql,[userid, JSON.stringify(newContent) ]);

        if(result.affectedRows > 0){
            res.json(newContent);
        }else{
            res.json({});
        }


        // var userid = req.session.userid ;
        // var sql = "select dom from `html` where userid=? " ;
        // var dataList = await query(sql,[userid]);
        
        // var dom = {}
        // if (dataList.length != 0){
        //     console.log("Old Dom: ",dataList[0].dom)
        //     dom = JSON.parse(dataList[0].dom);
        // }

        // lodash.defaultsDeep(dom,newContent);
        // console.log("New Dom: ",dom);

        // if(dataList.length == 0){
        //     var sql = "insert into `html` (`userid`,`dom`) values (?,?) ";
        //     var result = await query(sql,[userid, JSON.stringify(dom) ]);
        // }else{
        //     sql = "update `html` set `dom` = ? where `userid` = ?";
        //     console.log(JSON.stringify(dom));
        //     var result = await query(sql,[ JSON.stringify(dom),userid]);
        // }

        // if(result.affectedRows > 0){
        //     res.json(newContent);
        // }else{
        //     res.json({});
        // }

    }

});


var server = app.listen(80, function() {
    console.log('Listening on port %d', server.address().port);
});