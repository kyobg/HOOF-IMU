clc
clear all
close all
warning off
c = VideoReader('Horse-6.mp4');
%Specify that reading should begin 0.5 second from the beginning of the file by setting the CurrentTime property.
c.CurrentTime = 0.5;

while hasFrame(c)
    vidFrame = readFrame(c);
%   imshow(vidFrame);
    gray_vid = rgb2gray(vidFrame);
%   imshowpair(vidFrame, gray_vid, 'montage');
    edges_prewitt = edge(gray_vid,"prewitt");
    edges_roberts = edge(gray_vid,"roberts");
    edges_sobel = edge(gray_vid,"sobel");

    figure;
    subplot(1,4,1);
    imshow(edges_prewitt);
    subplot(1,4,2);
    imshow(edges_roberts);
    subplot(1,4,3);
    imshow(edges_sobel);
    subplot(1,4,4);
    imshow(gray_vid);
    linkaxes;



%   imshow(vidFrame)
%   mkdir = createMask6(vidFrame);

%   B=strel('square',10);
%   mkdir2 = imerode(mkdir,B);
%   mkdir3=imdilate(mkdir2, B);
%   B=strel('square',5);
%   mkdir2 = imopen(mkdir,B);
%   mkdir3 = imclose(mkdir2,B);

    %Image enhancement 
    %mkdir2 = imfill(mkdir,'holes');
    %mkdir3 = bwmorph (mkdir2,'dilate',3);
    %mkdir3 = imfill(mkdir3,'holes');
    %diplay the images

% the new method:
% mkdir4 = bwmorph(mkdir3,'skel',inf);
% imshowpair(vidFrame,mkdir4,'montage');

  pause(1/c.FrameRate);
end


