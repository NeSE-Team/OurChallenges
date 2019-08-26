#!/usr/bin/python
#coding:utf-8 


import os
import time

import selenium
from selenium import webdriver

chrome_options = webdriver.ChromeOptions()
chrome_options.add_argument('--headless')
chrome_options.add_argument('--disable-gpu')
chrome_options.add_argument('--disable-xss-auditor')
chrome_options.add_argument('--no-sandbox')
driverpath = '/usr/bin/chromedriver'
host= "http://127.0.0.1/"


username = "admin"
password = os.getenv("FLAG")

client = webdriver.Chrome(chrome_options=chrome_options,executable_path=driverpath)
client.set_page_load_timeout(10)
client.set_script_timeout(10)

print("[*] start chrome browser ......")

def login():

    client.get(host)
    client.set_page_load_timeout(10)
    client.set_script_timeout(10)

    try:
        client.switch_to_alert().accept()
    except selenium.common.exceptions.NoAlertPresentException:
        pass

    usernameForm = client.find_element_by_xpath("//input[@name='username']")
    passwordForm = client.find_element_by_xpath("//input[@name='password']")

    usernameForm.clear()
    passwordForm.clear()
    usernameForm.send_keys(username)
    passwordForm.send_keys(password)
    loginButton = client.find_element_by_xpath("//input[@type='submit']")
    loginButton.click()
    print(client.current_url)

    # content = driver.page_source.encode('utf-8')
    # print(content)

if __name__=='__main__':
    try:
        login()
    except Exception as e:
        pass
        # print(e)
    finally:
        client.quit()