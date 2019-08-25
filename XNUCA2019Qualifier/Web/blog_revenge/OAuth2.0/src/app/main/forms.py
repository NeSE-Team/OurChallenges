#!coding:utf-8 


from flask_wtf import Form 
from wtforms import StringField, PasswordField, SubmitField
from wtforms.validators import DataRequired, Length, Email,EqualTo,ValidationError
from .. import db
from ..models import User

class LoginForm(Form):
    email = StringField(u'邮箱',validators=[DataRequired(),Length(1,64),Email()])
    password = PasswordField(u'密码',validators=[DataRequired()])
    submit = SubmitField(u'登  录')

class Oauth(Form):
    email = StringField(u'邮箱',validators=[DataRequired(),Length(1,64),Email()])
    password = PasswordField(u'密码',validators=[DataRequired()])
    submit = SubmitField(u'授   权')
    
class RegisterForm(Form):
    email = StringField(u'邮箱',validators=[DataRequired(),Length(1,64),Email()])
    nickname = StringField(u'昵称',validators=[DataRequired(),Length(1,64)])
    password1 = PasswordField(u'密码',validators=[DataRequired()])
    password2 = PasswordField(u'重复密码',validators=[DataRequired(),EqualTo('password1')])
    submit = SubmitField(u'注  册')

    def validate_nickname(self, nickname):
        user = User.query.filter_by(nickname=nickname.data).first()
        if user is not None:
            raise ValidationError(u'昵称已经被注册!')
        
    def validate_email(self,email):
        user = User.query.filter_by(email=email.data).first()
        if user is not None:
            raise ValidationError(u'邮箱已经被注册!')

