clc
clear all
close all
warning off

% Read the video file
c = VideoReader('Horse-10.mp4');

% Specify that reading should begin 0.5 second from the beginning of the file by setting the CurrentTime property.
c.CurrentTime = 0.5;

% Set the line thickness for the skeleton
lineThickness = 5;

while hasFrame(c)
    % Read a frame from the video
    vidFrame = readFrame(c);
    
    % Create the skeleton mask
    mask = createMask2(vidFrame);
    B = strel('square', 5);
    skeleton = imopen(mask, B);
    skeleton = imfill(skeleton, 'holes');
    
    % Dilate the skeleton mask to make lines thicker
    dilatedSkeleton = imdilate(skeleton, strel('square', lineThickness));
    
    % Create an RGB image with white skeleton lines
    skeletonOverlay = uint8(255 * dilatedSkeleton);
    skeletonRGB = cat(3, skeletonOverlay, skeletonOverlay, skeletonOverlay);
    
    % Overlay the skeleton on the original frame
    overlayedImage = vidFrame + skeletonRGB;
    
    % Display the overlayed image
    imshow(overlayedImage);
    
    pause(1 / c.FrameRate);
end
