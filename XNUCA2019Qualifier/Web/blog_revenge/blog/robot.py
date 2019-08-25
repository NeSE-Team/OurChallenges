#coding:utf-8 

import selenium
from selenium import webdriver
import time
import os
import urlparse

chrome_options = webdriver.ChromeOptions()
chrome_options.add_argument('--headless')
chrome_options.add_argument('--disable-gpu')
chrome_options.add_argument('--disable-xss-auditor')
chrome_options.add_argument('--no-sandbox')
driverpath = '/usr/bin/chromedriver'
host= "http://"+os.environ.get('HOST')+"/"
oauthHost = os.environ.get('OAUTHSERVER')
username = "admin"
email = "admin@qq.com"
password = "LUUpoBCc7nM1HAJo"

client = webdriver.Chrome(chrome_options=chrome_options,executable_path=driverpath)
client.set_page_load_timeout(10)
client.set_script_timeout(10)

print("[*] start chrome browser ......")
def login(url=host+"main/login"):
    client.get(url) 
    usernameForm = client.find_element_by_xpath("//input[@name='username']")
    passwordForm = client.find_element_by_xpath("//input[@name='password']")
    usernameForm.clear()
    passwordForm.clear()
    usernameForm.send_keys(username)
    passwordForm.send_keys(password)
    loginButton = client.find_element_by_xpath("//button[@type='submit']")
    # print(loginButton)
    loginButton.click()
    print(client.current_url)
    # print(password)
def visiteBugUrl(url=host+"main/post"):
    client.get(url)
    print(client.current_url)
    urlElements = client.find_elements_by_xpath("//li[a/@target='_blank']")
    # print(urlElements)
    urls = []
    for urlElement in urlElements:
        urls.append(urlElement.text)
    for url in urls:
        print("[*] start to visite %s"%(url))
        try:
            client.get(url)
        except Exception as e:
            continue
        # 判断是否跳转到了oauth页面 
        currentUrl = client.current_url
        if urlparse.urlparse(currentUrl).netloc == oauthHost:
            print("[*] start to oauth %s"%(currentUrl))
            oauth()
                       
def oauth():
    emailForm = client.find_element_by_xpath("//input[@name='email']")
    passwordForm = client.find_element_by_xpath("//input[@name='password']")
    buttonForm = client.find_element_by_xpath("//input[@name='submit']")
    if emailForm is None or passwordForm is None:
        return
    else:
        emailForm.clear()
        passwordForm.clear()
        emailForm.send_keys(email)
        passwordForm.send_keys(password)
        buttonForm.click()
        try:
            client.switch_to_alert().accept()
        except selenium.common.exceptions.NoAlertPresentException:
            pass
        print("[*] I am visiting:%s"%(client.current_url))
if __name__=='__main__':
    try:
        login()
        visiteBugUrl()
    except Exception as e:
        print("[*] connect to host error!")
    finally:
        client.quit()