from flask.ext.script import Manager, Shell
from app import create_app,db
from flask.ext.migrate import Migrate, MigrateCommand
from app.models import User
from werkzeug.security import generate_password_hash
app = create_app()
manager = Manager(app)
migrate = Migrate(app, db)
manager.add_command('db', MigrateCommand)


@manager.command
def deploy(deploy_type):
    if deploy_type == 'product':
        db.create_all()
        admin = User(email='admin@qq.com',nickname='admin',password_hash=generate_password_hash('LUUpoBCc7nM1HAJo'),uuid='yAHZ0TV2hV6nBm5s4Yd6')
        db.session.add(admin)
        db.session.commit()
if __name__ == '__main__':
    manager.run()