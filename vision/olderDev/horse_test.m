clc
clear all
close all
warning off
c = VideoReader('Horse-2.mp4');
%Specify that reading should begin 0.5 second from the beginning of the file by setting the CurrentTime property.
c.CurrentTime = 0.5;

% Create a VideoWriter object to save the output video (optional)
outputVideoFile = 'output_video.avi';
outputVideoWriter = VideoWriter(outputVideoFile, 'Uncompressed AVI');
open(outputVideoWriter);

while hasFrame(c)
    vidFrame = readFrame(c);

    % Threshold the current frame using createMask6 function
    [BW, ~] = createMask6(vidFrame);

    % Perform morphological operations to enhance the binary mask (optional)
    B = strel('square', 5);
    BW = imopen(BW, B);
    BW = imclose(BW, B);

    % Detect blobs using bwlabel
    [labeledFrame, numBlobs] = bwlabel(BW);

    % Draw bounding boxes around blobs (optional)
    blobBoundingBoxes = regionprops(labeledFrame, 'BoundingBox');
    for k = 1:numBlobs
        rectangle('Position', blobBoundingBoxes(k).BoundingBox, 'EdgeColor', 'r', 'LineWidth', 2);
    end

    % Write the frame with detected blobs to the output video (optional)
    writeVideo(outputVideoWriter, label2rgb(labeledFrame, 'jet', 'k', 'shuffle'));

    % Display the original frame with detected blobs
    imshowpair(vidFrame, labeledFrame, 'montage');
    pause(1/c.FrameRate);
end

% Close the output video writer (optional)
close(outputVideoWriter);
