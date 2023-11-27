clc
clear all
close all
warning off

% Read the video file
c = VideoReader('Horse-10.mp4');
% Specify that reading should begin 0.5 second from the beginning of the file
c.CurrentTime = 0.5;

while hasFrame(c)
    vidFrame = readFrame(c);

    % Convert frame to grayscale and perform basic thresholding
    grayFrame = rgb2gray(vidFrame);
    binaryFrame = grayFrame > 100; % Adjust the threshold value as needed

    % Noise reduction using morphological operations
    se = strel('disk', 3); % Adjust the size of the structuring element
    binaryFrame = imopen(binaryFrame, se);

    % Thicken the line using morphological dilation
    se = strel('disk', 5); % Adjust the size of the structuring element for dilation
    thickenedLine = imdilate(binaryFrame, se);

    % Display the original frame and the processed line
    imshowpair(vidFrame, thickenedLine, 'montage');

    pause(1 / c.FrameRate);
end
