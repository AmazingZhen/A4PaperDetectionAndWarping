# A4PaperDetectionAndWarping
A solution for detecting A4 paper by canny, segmentation and hough transform and then warping it.
Based on [CImg Library](http://cimg.eu/).

## Input
- A picture in which A4 paper is as main part and has some degree of geometrical distortion.
- All input images are in the folder "dataset".
- ![sample input](https://github.com/AmazingZhen/A4PaperDetectionAndWarping/blob/master/EdgeDetection/dataset/13.jpg?raw=true)

## Output
- A standard 1190 * 1684 image, containing all contents on the A4 paper.
- All output images are in the folder "res".
- ![sample result](https://github.com/AmazingZhen/A4PaperDetectionAndWarping/blob/master/EdgeDetection/res/final_13.jpg?raw=true)

## Algorithmic process
- Edge extraction 
  - Two different edge extraction methods.
    + Canny edge extraction.
      + Calculate gradient intensity matrix and gradient direction of image.
      + Perform non-maximum suppression at the gradient intensity matrix.
      + Perform hysteresis thresholding to the suppression.
    + Image segmentation.
      + Binaryzation by thresholding and then obtain maximum connected domain by BFS.
      + Use laplacian edge detection.
      + Perform non-maximum suppression at the edge.
- Hough transform
  - Get line parameters in hough space from edge.
  - Select four credible line parameters to form a rectange and regard it as the whole A4 paper.
  - Calculate four vertices from four lines.
- Image Warping
  - Calculate homography matrix H (projective transformation) by four vertice-pairs.
  - Perform a linear transformation at each pixel to get the position after transform.
  - Use interpolation and backward mapping to form the output image.
