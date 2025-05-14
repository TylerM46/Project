#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  untit2.py
#  
#  Copyright 2025  <tyler@raspberrypi>

import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BCM)
eeptime=0.0001
pin=6
x=True
GPIO.setup(5, GPIO.OUT)
GPIO.setup(6, GPIO.OUT)

while x:
    
    
    GPIO.output(5,GPIO.HIGH)
    GPIO.output(6,GPIO.HIGH) 
   
    time.sleep(eeptime)  
 
    GPIO.output(5,GPIO.LOW)
    GPIO.output(6,GPIO.LOW)
    time.sleep(eeptime)
