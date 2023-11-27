%this one will try to combine multiple shorter lines into one bigger ones

c = VideoReader('Horse-5.mp4');
c.CurrentTime = 0.5;
while hasFrame(c)
    vidFrame = readFrame(c);
    [BW, ~] = createMask6(vidFrame);
    BW = imopen(imclose(BW, strel('square', 5)), strel('square', 5));
    [labeledFrame, numBlobs] = bwlabel(BW);
    blobMeasurements = regionprops(labeledFrame, 'Area');
    [~, idx] = max([blobMeasurements.Area]);
    largestBlobMask = ismember(labeledFrame, idx);
    largestBlobMaskUint8 = im2uint8(largestBlobMask);
    
    % Creating a continuous skeleton
    skeleton = bwmorph(largestBlobMask, 'skel', inf); % Initial skeleton
    skeleton = bwmorph(skeleton, 'spur', Inf); % Remove spur pixels to smoothen
    
    % Merge disconnected segments by dilation
    se = strel('disk', 1); % Adjust the size of the structuring element as needed
    mergedSkeleton = imdilate(skeleton, se); % Dilate to connect segments
    
    imshowpair(vidFrame, mergedSkeleton, 'blend');
end
