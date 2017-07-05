'''

 Author: Akshay U Hegde

 Description: This is the solution for Lab 4.

'''


import serial
import numpy as np
import matplotlib.pyplot as plt
from drawnow import drawnow

# Initialize X and Y plot variables
x = 0
y = 0

# Initialize interactive plotting
plt.ion()

# Function definition for drawnow()
def makeFig():
    plt.axis([0, 255, 0, 255])
    plt.scatter(x, y)
    plt.pause(0.001)

# Infinite Loop
while True:
    ser = serial.Serial('/dev/tty.usbmodem0E217F91', 9600)  # Read Serial Input
    incoming = ord(ser.read())
    if incoming == 126:  # Check Start Frame
        x = 255 - ord(ser.read())  # Read X-Axis ADC
        y = ord(ser.read())  # Read Y-Axis ADC
        drawnow(makeFig)  # Plot
