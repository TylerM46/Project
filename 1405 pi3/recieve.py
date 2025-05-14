#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  untit2.py
#  
#  Copyright 2025  <tyler@raspberrypi>

import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BCM)
eeptime=0.00001
pin=26
pinstates =[]
GPIO.setup(26, GPIO.IN)

for i in range(500):   
    
    pinstates.append(GPIO.input(26))
    
    time.sleep(eeptime)

print(pinstates)
