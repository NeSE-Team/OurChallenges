import requests
import time
import json
from db import Database
import os
from multiprocessing import Pool, Manager
from loguru import logger
import sys

ips = ["192.168.15.166"]

checkurl = 'http://192.168.100.100/Check/Index/putCheckLog' 
key = 'b07e3130-c023-11e9-8d27-6c92bfdfb66f'
process = 2
ans = Manager().list()
logger.remove()
logger.add(sys.stderr, level='INFO', enqueue=False)
logger.add('check.log', level='INFO', enqueue=False, encoding='utf-8')
dbone = Database('wasm')


def sendRequest(url,session,method,headers={},data=""):
    
    if method == "GET":

        try:
            res = session.get(url,headers=headers,params=data,timeout=5)
        except Exception as e:
            return None,0
        else:
            return res,1

    elif method == "POST":
        
        try:
            res = session.post(url,headers=headers,data=data,timeout=5)
        except Exception as e:
            return None,0
        else:
            return res,1

def checkPoint(ip):

    # GET / 
    url = "http://{}/".format(ip)
    session = requests.Session()

    res,ret = sendRequest(url,session,"GET")

    if ret != 1 or res.status_code != 200 or "Here is no note for you" not in res.text:
        return False

    # GET /info
    #  
    url = "http://{}/info".format(ip)
    session = requests.Session()

    res,ret = sendRequest(url,session,"GET")

    if ret != 1 or res.status_code != 200 or "hello world" not in res.text:
        return False

    # POST /urlnote 

    url = "http://{}/urlnote".format(ip)

    headers = {
        "Content-Type": "application/x-www-form-urlencoded"
    }

    payload = "title=urlnote&url=http://127.0.0.1:8080/"

    res,ret = sendRequest(url,session,"POST",headers,payload)
    # print(res.text)

    if ret!= 1 or res.status_code != 200 or "urlnote add success" not in res.text:
        return False
    
    url = "http://{}/".format(ip)

    res,ret = sendRequest(url,session,"GET")
    
    if ret!= 1 or res.status_code != 200 :
        return False
    
    # POST /textnote
    session = requests.Session()
    url = "http://{}/textnote".format(ip)

    headers = {
        "Content-Type": "application/x-www-form-urlencoded"
    }

    payload = "title=textnote&content=9ed05b47d45d3c1dda075a5a219a7a16"
    res,ret = sendRequest(url,session,"POST",headers,payload)
    
    if ret!= 1 or res.status_code != 200 or "textnote add success" not in res.text:
        return False
    
    url = "http://{}/".format(ip)

    res,ret = sendRequest(url,session,"GET")

    if ret!= 1 or res.status_code != 200 or "9ed05b47d45d3c1dda075a5a219a7a16" not in res.text :
        return False
    
    return True





def post_log(turn):
    anslist = list(ans)
    data = {'key':key, 'data': json.dumps(anslist)}
    tries = 3
    isSubmit = 0
    while tries>0:
        tries -= 1
        try:
            resp = requests.post(url=checkurl, data=data, timeout=5)
            res = json.loads(resp.text)
            isSubmit = res['code']
        except Exception as e:
            logger.error(f'error {type(e)}:{str(e)} occured when post log')
        if isSubmit:
            break
    if isSubmit == 0:
        logger.error(f'fail to submit log in {time.strftime("%Y-%m-%d-%H-%M-%S", time.localtime())}')
    for _ in anslist:
        dbone.addOne(_['ip'], time.strftime("%Y-%m-%d-%H-%M-%S", time.localtime()), _['status'], isSubmit, turn)
    return

def check(ip):
    isvisit = checkPoint(ip)
    if isvisit:
        logger.info(f'check finished at {ip}, status is success')
        ans.append({'ip':ip, 'status':1, 'time':int(time.time())})
        return True
    else:
        logger.info(f'check finished at {ip}, status is fail')
        ans.append({'ip':ip, 'status':0, 'time':int(time.time())})
        return False


if __name__ == '__main__':
    turn = 0
    while True:
        turn += 1
        logger.info(f'Starting Turn{turn} check')
        ans = Manager().list()
        start = time.time()
        pool = Pool(process)
        pool.map(check, ips)
        pool.close()
        post_log(turn)
        logger.info(f'Stopping Turn{turn} check, time cost {time.time() - start}')
        used = time.time() - start
        if used < 60:
            time.sleep(60 - used)