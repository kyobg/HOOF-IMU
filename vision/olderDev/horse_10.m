clc
clear all
close all
warning off
c = VideoReader('Horse-10.mp4');
% Specify that reading should begin 0.5 second from the beginning of the file by setting the CurrentTime property.
c.CurrentTime = 0.5;


while hasFrame(c)
    vidFrame = readFrame(c);
    mkdir = createMask2(vidFrame);

    B = strel('square', 5);
    mkdir2 = imopen(mkdir, B);
    mkdir3 = imclose(mkdir2, B);

    % Image enhancement 
    mkdir2 = imfill(mkdir, 'holes');
    mkdir3 = imfill(mkdir3, 'holes');
  
    % the new method:
    mkdir4 = bwmorph(mkdir3, 'skel', inf);
    
    % Change the color of the skeleton to red
    redSkeleton = cat(3, ones(size(mkdir4)), zeros(size(mkdir4)), zeros(size(mkdir4)));
    
    % Overlay the red skeleton on the original frame
    overlayedImage = vidFrame;
    overlayedImage(repmat(mkdir4, [1 1 3])) = redSkeleton(repmat(mkdir4, [1 1 3]));
    
    % Display the overlayed image
    imshow(overlayedImage);
    
    pause(1 / c.FrameRate);
end
