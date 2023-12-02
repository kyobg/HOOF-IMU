% Project: Hoof IMU
% Author: Kyle Garcia
% Script: Central Device Data Store
% Date: August 30, 2023
% Description: 
% Using a serial port, we can obtain the sensor readings and place them into a
% a .csv file for data visualization and for data processing later on.

clear all
clc
serialportlist("all");

%% SERIAL PORT DECLARATION 

% Serial Port Declaration: Contains a specific Baud Rate and a terminator
% to tell the program when to containue to the next line of string data
% using Line Feed and Carry Return.

dataArray = {};
serialportObj = serialport("COM3",115200);
configureTerminator(serialportObj,"CR/LF");

% Utilization of a callback function that uses a terminator to define a new
% data point

configureCallback(serialportObj, "terminator", @newDataPt);  

% Redefine the Baud Rate to match that of the Baud Rate for the 9 DOF IMU

serialportObj.BaudRate = 115200;

%% CALLBACK FUNCTION

function newDataPt(serialportObj,file,~)
    data1 = readline(serialportObj);
    dataArrayString = split(data1);
    dataArray = str2double(dataArrayString)';
    fprintf(data1);
    fprintf('\n');
    writematrix(dataArray, 'DataSample4.csv','Delimiter',',','WriteMode','append');
end
