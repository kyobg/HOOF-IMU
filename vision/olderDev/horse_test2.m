clc
clear all
close all
warning off
c = VideoReader('Horse-2.mp4');
% Specify that reading should begin 0.5 second from the beginning of the file by setting the CurrentTime property.
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

    % Measure properties of the blobs
    blobMeasurements = regionprops(labeledFrame, 'Area');

    % Find the index of the largest blob
    [~, idx] = max([blobMeasurements.Area]);

    % Create a binary mask with only the largest blob
    largestBlobMask = ismember(labeledFrame, idx);

    % Draw bounding box around the largest blob (optional)
    blobBoundingBox = regionprops(largestBlobMask, 'BoundingBox');
    rectangle('Position', blobBoundingBox.BoundingBox, 'EdgeColor', 'r', 'LineWidth', 2);

    % Write the frame with the largest blob to the output video (optional)
    writeVideo(outputVideoWriter, largestBlobMask);

    % Display the original frame with the largest blob
    imshowpair(vidFrame, largestBlobMask, 'montage');
    pause(1/c.FrameRate);
end

% Close the output video writer (optional)
close(outputVideoWriter);
