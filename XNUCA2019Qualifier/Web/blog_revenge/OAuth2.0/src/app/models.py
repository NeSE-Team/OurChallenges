from . import db
from flask.ext.login import UserMixin
from . import login_manager

class User(UserMixin,db.Model):
    __tablename__ = 'user'
    id = db.Column(db.Integer,primary_key=True)
    email = db.Column(db.String(64), unique=True, index=True)
    nickname = db.Column(db.String(64), unique=True, index=True)
    password_hash = db.Column(db.String(128))
    uuid = db.Column(db.String(40))

class Code(db.Model):
    __tablename__ = 'code'
    id = db.Column(db.Integer,primary_key=True)
    code = db.Column(db.String(40), index=True)
    user_id = db.Column(db.Integer)
    timeout = db.Column(db.Integer)

class Token(db.Model):
    __tablename__ = 'token'
    id = db.Column(db.Integer,primary_key=True)
    user_id = db.Column(db.Integer)
    token = db.Column(db.String(60), index=True)
    timeout = db.Column(db.Integer)

@login_manager.user_loader
def load_user(user_id):
    return User.query.get(int(user_id))