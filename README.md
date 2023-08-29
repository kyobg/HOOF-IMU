# Hoof-IMU Motion Analysis - Senior Design Project

**Preface:** Here are a couple links to help familiarize yourself with Git and GitHub.

Links: 

(1) How to use Git: https://youtu.be/USjZcfj8yxE?si=m7MtrsjsvbkFWDZs  

(2) How to use GitHub: https://www.youtube.com/watch?v=nhNq2kIvi9s&ab_channel=ColtSteele

## Terminology:

**IMU** - Inertial Measurement Unit

**DOF** - Degrees of Freedom

**Laminitis** - The inflammation of the laminae of the foot, which results in extreme pain and instability in horses.

## Project Synopsis:
A custom 9-DOF IMU with an additional 3-axis high-g accelerometer, which is used for impulses where the low-g accelerometer may clip, will be used to detect laminitis or rotational hoof sickness, in racehorses.
The IMUs come equipped with a 3-axis low-g accelerometer, 3-axis high-g accelerometer, 3-axis gyroscope, and 3-axis magnetometer. Additionally, circuitry will be added for other features like GPS, Real-time timekeeping, and power management.

The IMUs will communicate directly to a central hub through a BLE mesh, which will allow wireless transmission of recorded data from each unique IMU device. After data collection, the data will be read from the central into a serial port on a computer where it will save the data and provide a real-time plot for data processing later.


## Goals (September 13, 2023):
- Complete IMU data collection design
- Complete power management system
- Complete real-time plotting using KST2
- Complete data PowerPoint presentation
- Integrate GPS into the IMU system.
- Implement RTC

## Goals (After September 13):
- Improve the data packaging scheme
- Optimize power consumption
- Complete PCB design with integrated circuits and order appropriate chips
