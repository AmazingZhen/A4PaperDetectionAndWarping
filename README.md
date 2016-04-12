# A4PaperDetectionAndWarping
A solution for detecting A4 paper by canny, segmentation and hough transform and then warping it.
Based on [CImg Library](http://cimg.eu/).

## Input
- A picture in which A4 paper is as main part and has some degree of geometrical distortion.
- All input images are in the folder "dataset".

## Output
- A standard 1190 * 1684 image, containing all contents on the A4 paper.
- All output images are in the folder "res".

## Algorithmic process
- Two different edge extraction methods.
  + Canny, J., [A Computational Approach To Edge Detection](http://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=4767851), IEEE Trans. Pattern Analysis and Machine Intelligence, 8(6):679–698, 1986.
  + Image segmentation based on threshold, obtain maximum connected domain by BFS.
- Use [Hough transform](http://www.ai.sri.com/pubs/files/tn036-duda71.pdf) to obtain the equation of four lines.
- Calculate homography matrix H (projective transformation), then perform a linear transformation to correcting the geometrical distortion.
