# Central Device Data Store
# Kyle Garcia
# This is intended for use with KST2
'''
This is Python code specifically developed to replace
the function of the MATLAB verison of this code. Due to
MATLAB's proprietary nature, and awkward syntaxing for dealing
with COMPORTs, Python will be used.
'''
import sys
import serial
from serial.tools import list_ports
import csv


ports = list(list_ports.comports())
for p in ports:
    print(p)
    if len(ports) == 1:
        serialport = serial.Serial("COM3", baudrate=115200,bytesize=8)
    else:
        serialport = input("Enter COMPORT Name: ")

while True:
    with open('CurrentDataSamples.csv', 'a') as csvfile:
        writer = csv.writer(csvfile, delimiter=',',lineterminator='\r\n',strict=True)
        if serialport.in_waiting > 0:
            temp = serialport.readline()
            writer.writerow(temp)
        

