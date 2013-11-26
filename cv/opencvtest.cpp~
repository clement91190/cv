#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace cv;
using namespace std;

Mat src; Mat src_hsv;
int thresh = 100;
int max_thresh = 255;
RNG rng(12345);

/// Function header
void thresh_callback(int, void* );

/** @function main */
int main( int argc, char** argv )
{
  /// Load source image and convert it to gray
  src = imread( argv[1], 1 );

  /// Convert image to gray and blur it
 cvtColor( src, src_hsv, CV_BGR2HSV );

  /// Create Window
  char* source_window = "Source";
  char* source_hsv = "Source HSV";
 Size s = src.size();
  cout<< s <<endl;
  int d = src.channels();
    vector<Mat> layers;
   split(src_hsv, layers);
  namedWindow( source_window, CV_WINDOW_AUTOSIZE );
  namedWindow( source_hsv, CV_WINDOW_AUTOSIZE );
  //cout << layers << endl;
    Mat binar;

  int thresholdRed = 170;
  threshold(layers[0], binar, thresholdRed , 255 , 0 );

  Mat blur;

  medianBlur ( binar, blur, 5 );
 
  Mat contour_img = blur.clone();
  vector<vector<Point> > contours;
  findContours(contour_img, contours,CV_RETR_LIST, CV_CHAIN_APPROX_NONE );

  vector<Point> approxTriangle;
  for (int idx = 0; idx < contours.size(); idx++) {
          drawContours(src, contours, idx, Scalar(0,255,0));
          approxPolyDP(contours[i], approxTriangle, arcLength(Mat(contours[i]), true)*0.05, true);
        if(approxTriangle.size() == 3){
                  
            drawContours(src, contours, i, Scalar(255, 0, 0), CV_FILLED); // fill GREEN
                  }
  }
  
  
  imshow( source_window, src);
  imshow(source_hsv, blur );
  
  waitKey(0);
  return(0);
}

