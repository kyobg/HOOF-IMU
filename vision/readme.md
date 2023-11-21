![Logo](../Assets/Images/Horse.png)
# This Section of the Repository is for the IMU PROJECT - Vision section

This Repo will contains the development of the Computer Vision Aspect of this project
For more information of the MATLAB functions that are being used, please check our wiki page

===========================================================================

Information in regards of the MATLAB function being used for this project, their brief description, and help link to matlab help in alphabetical order:

- [bwlabel](https://www.mathworks.com/help/images/ref/bwlabel.html) - Label connected components in 2-D binary image
- [bwmorph](https://www.mathworks.com/help/images/ref/bwmorph.html) - Morphological operations on binary image
- [close](https://www.mathworks.com/help/matlab/ref/videowriter.close.html?searchHighlight=close&s_tid=srchtitle_support_results_3_close) - Close file / figures
- [hasFrame](https://www.mathworks.com/help/matlab/ref/videoreader.hasframe.html?searchHighlight=hasframe&s_tid=srchtitle_support_results_1_hasframe) - Determine if video frame is available to read
- [im2uint8](https://www.mathworks.com/help/images/ref/im2uint8.html?searchHighlight=im2uint8&s_tid=srchtitle_support_results_1_im2uint8) - Convert image to 8-bit unsigned integers
- [imclose](https://www.mathworks.com/help/images/ref/imclose.html?searchHighlight=imclose&s_tid=srchtitle_support_results_1_imclose) - Morphologically close image
- [imopen](https://www.mathworks.com/help/images/ref/imopen.html?searchHighlight=imopen&s_tid=srchtitle_support_results_1_imopen) - Morphologically open image
- [imshowpair](https://www.mathworks.com/help/images/ref/imshowpair.html?s_tid=doc_ta) - Compare differences between images
- [ismember](https://www.mathworks.com/help/matlab/ref/double.ismember.html?searchHighlight=ismember&s_tid=srchtitle_support_results_1_ismember) - Array elements that are members of set array
- [max](https://www.mathworks.com/help/matlab/ref/max.html?searchHighlight=max&s_tid=srchtitle_support_results_1_max) - Maximum element of array
- [open](https://www.mathworks.com/help/matlab/ref/videowriter.open.html?searchHighlight=open&s_tid=srchtitle_support_results_2_open) - Open file for writing video data
- [pause](https://www.mathworks.com/help/matlab/ref/pause.html?s_tid=doc_ta) - Stop MATLAB execution temporaily
- [readFrame](https://www.mathworks.com/help/matlab/ref/videoreader.readframe.html?s_tid=doc_ta) - Read next video frame
- [regionprops](https://www.mathworks.com/help/images/ref/regionprops.html?s_tid=doc_ta) - Measure properties of image regions
- [strel](https://www.mathworks.com/help/images/ref/strel.html?searchHighlight=strel&s_tid=srchtitle_support_results_1_strel) - Morphological structuring element; essential part of morphological dilation and erosion operation
- [writevideo](https://www.mathworks.com/help/matlab/ref/videowriter.writevideo.html?searchHighlight=writevideo&s_tid=srchtitle_support_results_1_writevideo) - Write video data to file
- [VideoReader](https://www.mathworks.com/help/matlab/ref/videoreader.html?s_tid=doc_ta) - Create an object to read video files
- [VideoWriter](https://www.mathworks.com/help/matlab/ref/videowriter.html?s_tid=doc_ta) - Create an object to write video files

===========================================================================

- [Morphological Operations](https://www.mathworks.com/help/images/morphological-filtering.html)

Morphology is a broad set of image processing operations that process images based on shapes. In a morphological operation, each pixel in the image is adjusted based on the value of other pixels in its neighborhood. By choosing the size and shape of the neighborhood, you can construct a morphological operation that is sensitive to specific shapes in the input image.

===========================================================================

Improvements and Targets:
- [x] Better color masking
- [x] Blob Analysis to minimize other objects
- [ ] Minimize the missing hoof masking (less mask perhaps)
- [ ] Cleaner skeleton with only main line showings
- [ ] Overlay the skeleton with lines to have a thicker and better skeletons
- [ ] Writing the video output as a file for ease of use
