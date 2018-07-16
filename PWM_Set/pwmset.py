import numpy as np
import matplotlib.pyplot as plt
import urllib.request
import json
import serial

ser = serial.Serial(
	    port='/dev/rfcomm0', #'/dev/ttyUSB0',#'/dev/ttyACM0',
	    baudrate=9600,
	    parity=serial.PARITY_NONE,
	    stopbits=serial.STOPBITS_ONE,
	    bytesize=serial.EIGHTBITS
	)

try:
    if ser.isOpen():
        ser.close()
    ser.open()
except Exception:
    print('Error abriendo el puerto')


while 1:
    pwm_r_n = int(input('Inserte PWM_r_n: '))
    dir_r_n = int(input('Inserte dir_r_n: '))
    pwm_v_b = int(input('Inserte PWM_v_b: '))
    dir_v_b = int(input('Inserte dir_v_b: '))
    
    if dir_r_n:
        pwm_r_n += 0x80

    if dir_v_b:
        pwm_v_b += 0x80

    if ser.isOpen():
        ser.write(b'\xFF')
        print(b'\xFF')
        ser.write(pwm_r_n.to_bytes(1, 'little', signed = False))
        print(pwm_r_n.to_bytes(1, 'little', signed = False))
        ser.write(pwm_v_b.to_bytes(1, 'little', signed = False))
        print(pwm_v_b.to_bytes(1, 'little', signed = False))

