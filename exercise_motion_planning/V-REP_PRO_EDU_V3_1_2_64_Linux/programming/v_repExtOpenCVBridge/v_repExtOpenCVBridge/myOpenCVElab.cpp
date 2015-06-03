/*
 * v-rep OpenCVBridge v1.1
 * by Marco "marcomurk" Bellaccini - marco.bellaccini[at!]gmail.com
 *
 * This software is provided "as is", without any warranty,
 * under the terms of the GNU LESSER GENERAL PUBLIC LICENSE, Version 3
 *
 */

/*OpenCV elaboration function example file*/

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "OCVbridgeType.h"

using namespace cv;

OCVBridge openCVElab(Mat ocvimage){
    /*-----------------------------------------*/
    //PUT YOUR IMAGE PROCESSING CODE HERE

    //code example

    //channels extraction
    std::vector<Mat> channels;
    Mat zeroMat;
    split(ocvimage, channels);
    zeroMat=channels[0]*0;

    //red binarization
    threshold( channels[2], channels[2], 200, 255, THRESH_BINARY );
    //green binarization
    threshold( channels[1], channels[1], 50, 255, THRESH_BINARY );
    //blue binarization
    threshold( channels[0], channels[0], 50, 255, THRESH_BINARY );

    //red thresholded image
    Mat red,ngreen,nblue;
    bitwise_not(channels[1],ngreen);
    bitwise_not(channels[0],nblue);
    bitwise_and(channels[2],ngreen,red);
    bitwise_and(red,nblue,red);



    //detect lines
    std::vector<Vec4i> lines;
    Canny(red, ocvimage, 20, 200, 3);

    HoughLinesP(ocvimage, lines, 5, CV_PI/180, 50, 60, 5 );

    cvtColor(ocvimage, ocvimage, CV_GRAY2BGR);

    //if a line was detected
    if(lines.size()>0){
       //draw line
       line( ocvimage,
             Point( lines.at(0)[0],lines.at(0)[1] ),
               Point( lines.at(0)[2],lines.at(0)[3] ),
               Scalar( 0, 0, 255 ),
               2,
               8 );
       //put text on the image
       putText(ocvimage, "Red border detected!",
               Point( cvRound(0.2*ocvimage.rows),
                      cvRound(0.9*ocvimage.rows)),
                      FONT_HERSHEY_SIMPLEX, 0.4,
                      Scalar( 0, 100, 255 ), 1, 8, false );
    } else {
        putText(ocvimage, "No red borders detected",
                Point( cvRound(0.2*ocvimage.rows),
                       cvRound(0.9*ocvimage.rows)),
                       FONT_HERSHEY_SIMPLEX, 0.4,
                       Scalar( 255, 0, 0 ), 1, 8, false );
    }



    //bridge return struct
    OCVBridge retStruct;

    //insert the resulting image in the bridge return struct
    retStruct.img=ocvimage;


    //HERE PUT YOUR OPTIONAL RETURN VALUES (a vector of floats)
    // That auxiliary information are retrieved with simHandleVisionSensor/simReadVisionSensor
    if(lines.size()>0 && lines.at(0)[0]<0.75*ocvimage.cols && lines.at(0)[2]<0.75*ocvimage.cols){
        retStruct.returnValues.push_back(1);
    } else {
        retStruct.returnValues.push_back(0);
    }


    /*-----------------------------------------*/

    //return the resulting bridge struct
    return retStruct;

}
