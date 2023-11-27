clc
clear all
close all
warning off
c = VideoReader('Horse-2.mp4');
%Specify that reading should begin 0.5 second from the beginning of the file by setting the CurrentTime property.
c.CurrentTime = 0.5;

while hasFrame(c)
    vidFrame = readFrame(c);
   % imshow(vidFrame)
    mkdir = createMask(vidFrame);

%     B=strel('square',10);
%     mkdir2 = imerode(mkdir,B);
%     mkdir3=imdilate(mkdir2, B);
    B=strel('square',5);
    mkdir2 = imopen(mkdir,B);
    mkdir3 = imclose(mkdir2,B);

    %Image enhancement 
    %mkdir2 = imfill(mkdir,'holes');
    %mkdir3 = bwmorph (mkdir2,'dilate',3);
    %mkdir3 = imfill(mkdir3,'holes');
    %diplay the images
    imshowpair(vidFrame,mkdir3,'montage');

    pause(1/c.FrameRate);
end

% e=imread ("Image-1.jpg");
