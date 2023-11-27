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

    % Adaptive thresholding to handle varying lighting conditions and noise levels
    threshold = adaptthresh(vidFrame, 0.3, 'NeighborhoodSize', 15);
    mkdir = imbinarize(vidFrame, threshold);

    % Noise reduction and line enhancement
    B = strel('square', 10);
    mkdir2 = imopen(mkdir, B); % Opening operation to remove noise
    mkdir3 = imclose(mkdir2, B); % Closing operation to connect broken lines and fill gaps

    % Thickening the line using dilation
    se = strel('square', 5);
    mkdir4 = imdilate(mkdir3, se);

    % Remove small isolated pixels or lines
    minLineLength = 50; % Define the minimum line length threshold
    mkdir4 = bwareaopen(mkdir4, minLineLength);
   
    % Fill holes in the line
    mkdir4 = imfill(mkdir4, 'holes');

    % Display the original frame and the processed line
    imshowpair(vidFrame, mkdir4, 'montage');

    pause(1 / c.FrameRate);
end
