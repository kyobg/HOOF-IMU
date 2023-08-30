% Project: Hoof IMU
% Author: Kyle Garcia
% Script: IMU Visualization for 9 DOF IMU
% Date: May 5, 2023
% Description: 
% Using a serial port, we can obtain a visualization of the 9 DOF IMU's
% orientation in real time, this will act as a means of visualization for
% each IMU later in the project, this is more so a proof of concept until
% BLE communication is realized and robust.

clear all
clc
serialportlist("all");

%% SERIAL PORT DECLARATION 

% Serial Port Declaration: Contains a specific Baud Rate and a terminator
% to tell the program when to containue to the next line of string data
% using Line Feed and Carry Return.
%fuse = ahrsfilter();
dataArray = {};
serialportObj = serialport("COM3",9600);
configureTerminator(serialportObj,"CR/LF");

% Utilization of a callback function that uses a terminator to define a new
% data point
configureCallback(serialportObj, "terminator", @newDataPt);  
%file = fopen("DataSample1.csv","a");
% Redefine the Baud Rate to match that of the Baud Rate for the 9 DOF IMU
serialportObj.BaudRate = 115200;

%% OLD CODE

% Read string data up to and including the first occurrence of the read terminator "LF"
% using the serialport object serialportObj. Data is returned without the read
% terminator.
%
% data1 = readline(serialportObj);
% dataArrayString = split(data1);
% dataArray = str2double(dataArrayString);
% ----------------------------------------------------------------------------------------
% 
% IMU = imuSensor('accel-gyro-mag');
% fuse = ahrsfilter();
% Read the first 3 16-int as the accel reading
% 
% AccelReading = [dataArray(2),dataArray(3),dataArray(4)];
% 
% Read the next 3 16-int as the gyroscope reading
% 
% GyroReading = [dataArray(5),dataArray(6),dataArray(7)];
% 
% Read the next 3 16-int as the magnetometer reading
% 
% MagReading = [dataArray(8),dataArray(9),dataArray(10)];
% 
% Read the next 3 16-int as second accel reading
% 
% Accel2Reading = [dataArray(11),dataArray(12),dataArray(13)];
% 
% imuFunc = IMU(AccelReading, GyroReading, MagReading);
% 
% q = fuse(AccelReading, GyroReading, MagReading);
% 
% q = fuse(AccelReading, GyroReading, MagReading);
% 
% pp = poseplot(q);

%% CALLBACK FUNCTION
% Edits made: 8/21/2023 2:00 PM
% Kyle: Added the Old Code into the callback function in the hopes that a
% new pose plot will be generate in place of the old one at each data point

function newDataPt(serialportObj,file,~)
    data1 = readline(serialportObj);
    dataArrayString = split(data1);
    dataArray = str2double(dataArrayString)';
    fprintf(data1);
    fprintf('\n');
    writematrix(dataArray, 'DataSample3.csv','Delimiter',',','WriteMode','append');
end