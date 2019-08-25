#! coding:utf-8 

class Config():
    # DEBUG = True
    SQLALCHEMY_DATABASE_URI = 'mysql://root:Cy4Qndv95KKrZCT1@mysql/oauth?utf-8'
    SQLALCHEMY_COMMIT_ON_TEARDOWN = True
    SQLALCHEMY_TRACK_MODIFICATIONS = True
    SECRET_KEY = 'this is a secret key to protect from csrf'
    WTF_CSRF_SECRET_KEY = 'this is a random key for form' # for csrf protection
    # 邮箱配置
    CLIENT_ID = "3f66bf84f42fec8fd6348593ab74db04"
    CLIENT_SECRET = '51abb9636078defbf888d8457a7c76f85c8f114c'
    REDIRECT_URL='http://__IP_AND_PORT__/main/oauth/'