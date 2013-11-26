#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <algorithm>
#include <curl/curl.h>

using namespace cv;
using namespace std;

class Feu
{
    private:
    static int objectCount;
    int ximg, yimg; // coordonne pixel image centre de gravite
    int couleur ; // 0 -> jaune, 1 -> rouge
    double confiance ; // entre 0 et 1 
    double last_perimeter;
    int last_frame;
    int id;
    public:
    Feu(int ximg_, int yimg_, int perimeter_,  int frame_num);
    bool is_the_same(int ximg_, int yimg_, int perimeter_, int frame_num);
    bool trust();
    bool clean(int frame_num);
    int getx();
    int gety();
    int getId();
};
// Initialize static member of class Box
int Feu::objectCount = 0;

Feu::Feu(int ximg_, int yimg_, int perimeter_, int frame_num):
    last_frame(frame_num), ximg(ximg_), yimg(yimg_),last_perimeter(perimeter_), confiance(0.5), id(objectCount)
{
    objectCount++;
}


int Feu::getId(){
    return id;
}

int Feu::getx(){
    return ximg;
}

int Feu::gety(){
    return yimg;
}

bool Feu::trust()
{
    return confiance > 0.9;
}
bool Feu::is_the_same(int ximg_, int yimg_, int perimeter_, int frame_num){
        // check if the fire is this one 
        double d = pow((ximg - ximg_), 2.0)  + pow((yimg - yimg_), 2.0) ;
        double dist_threshold =  100;
        double update = 0.9;
        if (d <= dist_threshold)
        {
            ximg = ximg * (1 - update) + ximg_ * update;
            yimg = yimg * (1 - update) + yimg_ * update;
            confiance +=(1.0 -  confiance)  / 3.0 ;
            last_frame = frame_num;
            last_perimeter = perimeter_;
                return true;
        }
        else
        {
            return false;
        }
           
    }


bool Feu::clean(int frame_num){
    if (frame_num > last_frame)
    {
        confiance = confiance * confiance;
    }
    return (confiance > 0.1);
}

void treat_frame(Mat src_full, char* source_window, char* source_hsv, vector<Feu>& fires, int frame_num);

    /** @function main */
int main (int argc, char **argv)
{

    
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    Mat src_full;
 /// Load source image and convert it to gray
    /// Create Window
    char *source_window = "Source";
    char *source_hsv = "Source HSV";
    int frame_num(0);
    vector<Feu> fires;
   //src_full = imread (argv[1], 1);
    namedWindow (source_window, CV_WINDOW_AUTOSIZE);
    namedWindow (source_hsv, CV_WINDOW_AUTOSIZE);
    
  VideoCapture cap;
  cap.open(0);
  for (;;)
  {
    cap >> src_full;
    if(!src_full.data) {
        cout << " no data " << endl;
        break;
    }
        
    if(waitKey(30) >= 0) break;
    treat_frame( src_full, source_window, source_hsv, fires, frame_num);
    vector<Feu>::iterator feu;
    int num_fires(0);
    for (feu = fires.begin(); feu != fires.end();)
    {
        if (!feu->clean(frame_num))
        {
            fires.erase(feu);
        }
        else
        {
            ++feu;
        }
        if (feu->trust())
        {
            if(curl){
                stringstream ss;
                ss <<  "http://127.0.0.1:5000/add_fire?x=" << feu->getx() <<"&y="<< feu->gety() << "&id=" << feu->getId();
                string url ;
                url = ss.str();
                curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                res = curl_easy_perform(curl); 
            }
            num_fires++ ;
        }

    }

    cout <<" nb of fires : " << num_fires << endl;  
    frame_num ++;
  }

}

void treat_frame(Mat src_full, char* source_window, char* source_hsv, vector<Feu>& fires, int frame_num)
{
    Mat src_hsv, src;
    int nW = 320;
    int nH = 240;
    resize( src_full, src, Size(320, 240),0,0 , CV_INTER_CUBIC );

    /// Convert image to gray and blur it
    cvtColor (src, src_hsv, CV_BGR2HSV);

   Size s = src.size ();
    int d = src.channels ();
    vector < Mat > layers;
    split (src_hsv, layers);
   //cout << layers << endl;
    Mat binar;

    int thresholdRed = 170;
    threshold (layers[0], binar, thresholdRed, 255, 0);

    Mat blur;

    medianBlur (binar, blur, 5);

    Mat contour_img = blur.clone ();
    vector < vector < Point > >contours;
    vector < vector < Point > >triangles;
    findContours (contour_img, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
    vector < Point > approxTriangle;
    for (int idx = 0; idx < contours.size (); idx++)
        {
        //drawContours (src, contours, idx, Scalar (0, 255, 0));
        double perimeter = arcLength ( Mat (contours[idx] ), true ) * 0.1;


        double poly_threshold ;
        poly_threshold = min( perimeter, 5.0 );


        approxPolyDP (contours[idx], approxTriangle, perimeter, true);
           if (perimeter > 5 && perimeter < 30&& approxTriangle.size () == 3)
        {

        drawContours (src, contours, idx, Scalar (255, 0, 0), CV_FILLED);	// fill GREEN
    //	}*/
        Point center(0,0);
        vector<Point>::iterator vertex;
            for(vertex = approxTriangle.begin(); vertex != approxTriangle.end(); ++vertex){
                 center += *vertex;       
            }

            center *= 0.33;
  //
        //Tracking fires
        vector<Feu>::iterator feu;
        bool new_fire(true);
        for (feu = fires.begin(); feu != fires.end(); ++feu)
        {
            if (feu->trust())
            {
            circle(src, center, 10, Scalar(255, 255, 255), -1.0, 8 );
            }
            else
            {
            circle(src, center, 5, Scalar(0, 255, 0), 1);
            }
            if (feu->is_the_same(center.x, center.y, perimeter, frame_num ))
            {
                new_fire = false;
                break;
            }    
        }
        if (new_fire)
        {
            fires.push_back(Feu(center.x, center.y, perimeter, frame_num));
        }
    

        }

    }
    




    imshow (source_window, src);
    imshow (source_hsv, layers[1]);

}



