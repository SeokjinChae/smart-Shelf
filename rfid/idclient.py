from socket import *
#!/usr/bin/env python
import RPi.GPIO as GPIO
from mfrc522 import SimpleMFRC522
import time


clientSock = socket(AF_INET, SOCK_STREAM)
clientSock.connect(('192.168.103.6', 8888))

print('연결 확인 됐습니다.')

question = 'y'
while(question == 'y'):
 reader = SimpleMFRC522()
 id, text = reader.read()
 print(text)
 text = text.strip()
 clientSock.send(text.encode('utf-8'))
 time.sleep(1)
 question = input('다음 책을 입력하시겠습니까? (y or n) : ')
 
