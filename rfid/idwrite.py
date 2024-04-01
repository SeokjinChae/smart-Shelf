#!/usr/bin/env python
import RPi.GPIO as GPIO
from mfrc522 import SimpleMFRC522

reader = SimpleMFRC522()

try:
        text = input('enter the name of book :')
        print("now place your book to write")
        reader.write(text)
        print("written")

finally:
        GPIO.cleanup()