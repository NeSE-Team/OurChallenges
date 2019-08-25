#coding:utf-8 

from flask import render_template , redirect , request , url_for ,flash,abort
from flask.ext.login import login_user, login_required, logout_user,current_user
from . import main
from .forms import LoginForm,RegisterForm,Oauth
from werkzeug.security import generate_password_hash, check_password_hash
from ..models import User,Code,Token
from .. import db
import string
import random
from .. import mail,Message
from flask import current_app
import urllib
import time
import json
from urlparse import urlparse
from urlparse import urlunparse
from posixpath import normpath

create_uuid = lambda x: ''.join([random.choice(string.ascii_letters + string.digits) for _ in range(x)])

@main.route('/')
@login_required
def index():
    return render_template('index.html',user=current_user)

@main.route('/logout')
@login_required
def logout():
    logout_user()
    return redirect(url_for('.login'))

@main.route('/login',methods=['GET',"POST"])
def login():
    # print(current_app.config['MAIL_SERVER'])
    if current_user.is_authenticated:
        return redirect(url_for('.index'))
    form = LoginForm()
    if form.validate_on_submit():
        email = form.email.data
        password = form.password.data
        user = User.query.filter_by(email=email).first()
        if user is None:
            flash(u'用户名或者密码错误！')
        elif not check_password_hash(user.password_hash,password):
            flash(u'用户名或者密码错误！')
        else:
            login_user(user)
            return redirect(request.args.get('next') or url_for('.index'))
    return render_template('login.html',form=form,name=u"登录")

def standUrl(url):
    if url is None:
        return ''
    url = urllib.unquote(url)
    res = urlparse(url)
    # fix rebirth find bug : use ..\ to by pass this .
    path = res.path
    path = path.replace("\\","/")
    path = normpath(path)
    # fix rebirth find bug : use ..\ to by pass this .
    
    return urlunparse((res.scheme, res.netloc, path+"/", res.params, res.query, res.fragment))

@main.route('/oauth',methods=['GET',"POST"])
def oauth():
    # print(current_app.config['MAIL_SERVER'])
    client_id = request.args.get('client_id',None)
    redirect_uri = standUrl(request.args.get('redirect_uri',None))
    scope = request.args.get('scope',None)
    response_type = request.args.get('response_type',None)

    
    if current_app.config['CLIENT_ID'] != client_id:
        abort(404)
    elif not redirect_uri.startswith(current_app.config['REDIRECT_URL']) :
        return redirect(current_app.config['REDIRECT_URL']+"?error=redirect_uri_mismatch&error_description=The+redirect_uri+MUST+match+the+registered+callback+URL+for+this+application.")
    elif response_type != 'code':
        abort(404)
    else:
        form = Oauth()
        if form.validate_on_submit():
            email = form.email.data
            password = form.password.data
            user = User.query.filter_by(email=email).first()
            if user is None:
                flash(u'用户名或者密码错误！')
            elif not check_password_hash(user.password_hash,password):
                flash(u'用户名或者密码错误！')
            else:
                code = Code(user_id=user.id,code=create_uuid(40),timeout=time.time()+60*5) # 5分钟过期
                db.session.add(code)
                db.session.commit()
                return redirect(redirect_uri+"?state="+create_uuid(10)+"&code="+code.code)
                
    return render_template('login.html',form=form,name=u"授权")

@main.route('/user',methods=['GET',"POST"])
def user():
    access_token = request.args.get('access_token',None)
    if access_token is None:
        msg = { "message": "Bad Token"}
        return json.dumps(msg)
    else:
        token = Token.query.filter_by(token=access_token).first()
        if token is None:
            msg = { "message": "Bad Token"}
            return json.dumps(msg)
        elif int(token.timeout) < time.time() :
            msg = {"message":"Token expired"}
            return json.dumps(msg)
        else:
            user_id = token.user_id
            user = User.query.filter_by(id=user_id).first()
            msg = {"email":user.email,"nickname":user.nickname,"uuid":user.uuid}
            return json.dumps(msg)

@main.route('/access_token',methods=['GET',"POST"])
def token():
    client_id = request.args.get('client_id',None)
    client_secret = request.args.get('client_secret',None)
    code = request.args.get('code',None)
    redirect_uri = request.args.get('redirect_uri',None)

    if current_app.config['CLIENT_ID'] != client_id:
        msg = {'error':'bad_client_id','error_description':'The client id is incorrect.'}
        return json.dumps(msg)

    elif current_app.config['CLIENT_SECRET'] != client_secret:
        msg = {'error':'bad_client_secret','error_description':'The client secret is incorrect.'}
        return json.dumps(msg)
    elif code is not None:
        code = Code.query.filter_by(code=code).first()
        if code is None or int(code.timeout) < time.time():
            msg = {'error':'bad_verification_code','error_description':'The code passed is incorrect or expired.'}
            return json.dumps(msg)
        else:
            user_id = code.user_id
            db.session.delete(code)
            access_token = create_uuid(60)
            token = Token(user_id=user_id,token=access_token,timeout=time.time()+60*20)
            db.session.add(token)
            db.session.commit()
            msg = {'access_token':access_token,'scope':'user','token_type':'bearer'}
            return json.dumps(msg)

    else:
        msg = {'error':'bad_verification_code','error_description':'The code passed is incorrect or expired.'}
        return json.dumps(msg)

@main.route('/register',methods=['GET',"POST"])
def register():
    if current_user.is_authenticated:
        return redirect(url_for('.index'))
    form = RegisterForm()
    if form.validate_on_submit():
        email = form.email.data
        nickname = form.nickname.data
        password_hash = generate_password_hash(form.password1.data)
        uuid = create_uuid(10)
        
        user = User(email=email,nickname=nickname,password_hash=password_hash,uuid=uuid)
        db.session.add(user)
        db.session.commit()
        flash(u'注册成功,你可以用这个账号授权第三方应用了!')
    return render_template('register.html',form=form)