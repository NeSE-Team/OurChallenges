import sqlite3
from loguru import logger
import sys


logger.remove()
logger.add(sys.stderr, level='INFO', enqueue=False)
logger.add('check.log', level='INFO', enqueue=False, encoding='utf-8')


class Database():
    def __init__(self, name):
        self.dbname = f'{name}.sqlite3'
        self.conn = sqlite3.connect(self.dbname)
        self.tablename = f'{name}_log'
        try:
            c = self.conn.cursor()
            c.execute(f'''CREATE TABLE IF NOT EXISTS {self.tablename}
                (ID INTEGER PRIMARY KEY AUTOINCREMENT,
                IP TEXT NOT NULL,
                TIME TEXT NOT NULL,
                CHECKSTATUS INTEGER NOT NULL,
                SUBMITSTATUS INTEGER NOT NULL,
                TURN INTEGER NOT NULL);''')
            self.conn.commit()
        except Exception as e:
            logger.error(f'error {type(e)}:{str(e)} occured when create')


    def addOne(self, ip, time, check, submit, turn):
        try:
            sql = f"insert into {self.tablename} (IP, TIME, CHECKSTATUS, SUBMITSTATUS, TURN) VALUES ('{ip}', '{time}', {check}, {submit}, {turn});"
            c = self.conn.cursor()
            c.execute(sql)
            self.conn.commit()
        except Exception as e:
            logger.error(f'error {type(e)}:{str(e)} occured when add record')