%Showing potential
%this method remove the innter skeleton and show the outer one instead

clc
clear all
close all
warning off

c = VideoReader('Horse-1.mp4');
c.CurrentTime = 0.5;

while hasFrame(c)
    vidFrame = readFrame(c);
    [BW, ~] = createMask6(vidFrame);
    B = strel('square', 5);
    BW = imopen(BW, B);
    BW = imclose(BW, B);
    [labeledFrame, numBlobs] = bwlabel(BW);
    blobMeasurements = regionprops(labeledFrame, 'Area');
    [~, idx] = max([blobMeasurements.Area]);
    largestBlobMask = ismember(labeledFrame, idx);
    largestBlobMaskUint8 = im2uint8(largestBlobMask);

    mkdir4 = bwmorph(largestBlobMask,'skel',inf);
    mkdir5 = bwmorph(mkdir4, 'skel', 'branchpoints');
    imshowpair(vidFrame,mkdir5,'blend');

    skelD = zeros(size(largestBlobMask));
    [y,x] = find(bwmorph(mkdir5, 'branchpoints'));
    [yEnd, xEnd] = find(bwmorph(mkdir5, 'endpoints'));
    B_loc = sub2ind(size(largestBlobMask), y, x);

    for k = 1:numel(xEnd)
        D = bwdistgeodesic(mkdir4, xEnd(k), yEnd(k));
        distanceToBranchPt = min(D(B_loc));
        skelD(D < distanceToBranchPt) = 1;
    end

    figure;
    imshowpair(vidFrame,skelD,'blend');
    hold on;
    plot(x, y, 'ro');
    hold off;

    pause(1/c.FrameRate);
end
