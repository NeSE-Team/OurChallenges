from flask import Flask
from config import Config
from flask.ext.sqlalchemy import SQLAlchemy
from flask_mail import Mail, Message
from flask_login import LoginManager

login_manager = LoginManager()
login_manager.session_protection = 'strong'
login_manager.login_view = 'main.login'


mail = Mail()
db = SQLAlchemy()

def create_app():
    app = Flask(__name__)

    app.config.from_object(Config)
    db.init_app(app)
    mail.init_app(app)
    login_manager.init_app(app)
    
    from .main import main as main_blueprint
    app.register_blueprint(main_blueprint)

    return app