#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "stdint.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>


#include "x264/stdint.h"
#include "x264/config.h"
#import "x264/config.h"
#import "x264/x264.h"
#include "x264_encoder.h"


using namespace cv;


int main() {


    CvCapture *capture = cvCreateCameraCapture(0);
    IplImage *frame;


//    cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
//    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

    int bufsize;
    x264Encoder x264;

    x264.Create(640, 480, 3, 25);

    uint8_t* buf;
    while (1){
        frame = cvQueryFrame(capture);

        Mat mat_img=cvarrToMat(frame);




        if (!mat_img.empty()){
            bufsize = x264.EncodeOneFrame(mat_img);
            if (bufsize > 0){
                buf = x264.GetEncodedFrame();
                printf("%d \n",strlen((char *)buf));
            }
        }



        if(waitKey(33) == 27) break;
    }

    cvReleaseCapture(&capture);
    cvDestroyWindow("hebiao");

    return 1;
}



void camera_show(){


    CvCapture *capture = cvCreateCameraCapture(0);
    IplImage *frame;

    int frameH    = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
    int frameW    = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);


    int yuv_bufLen = frameW*frameH*3/2;
    unsigned char* pYuvBuf = new unsigned char[yuv_bufLen];

    while (1){
        frame = cvQueryFrame(capture);

        Mat mat_img=cvarrToMat(frame);



        Mat yuvImg;

        cvtColor(mat_img, yuvImg, CV_BGR2YUV_I420);
        memcpy(pYuvBuf, yuvImg.data, yuv_bufLen*sizeof(unsigned char));

        IplImage yuv_frame = IplImage(yuvImg);
        cvShowImage("hebiao", &yuv_frame);






        if(waitKey(33) == 27) break;
    }

    cvReleaseCapture(&capture);
    cvDestroyWindow("hebiao");
}



