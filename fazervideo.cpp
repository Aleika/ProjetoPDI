#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<sstream>
#include <vector>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include "opencv2/opencv.hpp"
#include<string.h>
using namespace cv;
using namespace std;

int main()
{
    //Read sequence of images
    Size frameSize(1080,1080);

    String folder = "/home/tayna/Documentos/toopencv/*.jpg";
    vector<String> filenames;
 
    glob(folder, filenames);

    Mat myImage;
    cout<<filenames.size();

 
    for (size_t i = 0; i < filenames.size(); ++i)
    {
        myImage = imread(filenames[i]);
        cv::resize(myImage, myImage, frameSize);//all images will be saved the same size
        std::string savingName = "/home/tayna/Documentos/toopencv/resultado/" + std::to_string(i) + ".jpg";
        cv::imwrite(savingName, myImage);
        
    }
    //Write video
    VideoWriter out
    ("/home/tayna/Documentos/toopencv/resultado/sorrisos.avi",CV_FOURCC('P','I','M','1'), 24, frameSize,true);
  

    //writing images in the video


	String folderdst = "/home/tayna/Documentos/toopencv/resultado/*.jpg";
    vector<String> filenamesdst;
 
    glob(folderdst, filenamesdst);



    vector<Mat> img;//vector to store all the images
     for (size_t i = 0; i < filenamesdst.size(); ++i)
    { 
	img.push_back(imread(filenamesdst[i]));
		
    }
    vector<Mat>::iterator it;//iterator to the vector<Mat>

    for (it = img.begin(); it != img.end() ; it++) {
	out.write((*it));
    }
    
    return 1;
}


