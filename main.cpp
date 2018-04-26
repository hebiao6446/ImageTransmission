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


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <zconf.h>

#include <errno.h>

#define BUFLEN 512
#define NPACK 10
#define PORT 9999



using namespace cv;


struct client
{
    int host;
    short port;
};

// Just a function to kill the program when something goes wrong.
void diep(char *s)
{
    perror(s);
    exit(1);
}




int main() {

    /*

    CvCapture *capture = cvCreateCameraCapture(0);
    IplImage *frame;

//    cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
//    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

    int frameH    = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
    int frameW    = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);



    int bufsize;
    x264Encoder x264;
//    uint8_t* buf;
    x264.Create(frameW, frameH, 3, 25);




*/




    struct sockaddr_in si_me, si_other;
    int s, i, j;
    socklen_t slen =  sizeof(si_other);
    char buf[BUFLEN];
    struct client clients[10];
    int n = 0;
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
        diep("socket");
    memset((char *) &si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(s, (struct sockaddr*)(&si_me), sizeof(si_me))==-1)
        diep("bind");



    client c_;

    while (1){

        if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr*)(&si_other), &slen)!=-1){

            printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
            c_.host = si_other.sin_addr.s_addr;
            c_.port = si_other.sin_port;
            

            break;
        }


    }



    while (1){
//        frame = cvQueryFrame(capture);
//        Mat mat_img=cvarrToMat(frame);



        /*
        frame = cvQueryFrame(capture);
        Mat mat_img=cvarrToMat(frame);
        if (!mat_img.empty()){
            bufsize = x264.EncodeOneFrame(mat_img);


            if (bufsize > 0){
                buf = x264.GetEncodedFrame();
                printf("%s \n",(char *)buf);
            }
        }

        if(waitKey(25) == 27) break;
        if (mat_img.empty()){
            break;
        }

         */




        /*
        if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr*)(&si_other), &slen)==-1)
            diep("recvfrom");


        printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        clients[n].host = si_other.sin_addr.s_addr;
        clients[n].port = si_other.sin_port;
        n++;
        for (i = 0; i < n; i++)
        {
            si_other.sin_addr.s_addr = clients[i].host;
            si_other.sin_port = clients[i].port;
            for (j = 0; j < n; j++)
            {
                printf("Sending to %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
                if (sendto(s, &clients[j], 6, 0, (struct sockaddr*)(&si_other), slen)==-1)
                    diep("sendto");
            }
        }
        printf("Now we have %d clients\n", n);


       */






            si_other.sin_addr.s_addr = c_.host;
            si_other.sin_port = c_.port;

            printf("Sending to %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));

           char  *msg = "这是一段代码测试，看看字符串中文行不行";
           size_t lenth =  strlen(msg);
           if (sendto(s, msg, lenth, 0, (struct sockaddr*)(&si_other), slen)==-1)
                    diep("sendto");





//        if(waitKey(25) == 27) break;


        if(waitKey(250) == 27) break;
    }




//    cvReleaseCapture(&capture);
//    cvDestroyWindow("hebiao");

    return 1;
}





void camera_show(){


    CvCapture *capture = cvCreateCameraCapture(0);
    IplImage *frame;

    int frameH    = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
    int frameW    = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);


    int yuv_bufLen = frameW*frameH*3/2;
    unsigned char* pYuvBuf = new unsigned char[yuv_bufLen];


    int bufsize;
    x264Encoder x264;

    x264.Create(frameW, frameH, 3, 25);


    uint8_t* buf;



    while (1){
        frame = cvQueryFrame(capture);

        Mat mat_img=cvarrToMat(frame);



        Mat yuvImg;

        cvtColor(mat_img, yuvImg, CV_BGR2YUV_I420);
        memcpy(pYuvBuf, yuvImg.data, yuv_bufLen*sizeof(unsigned char));

        IplImage yuv_frame = IplImage(yuvImg);
        cvShowImage("hebiao", &yuv_frame);




        if (!mat_img.empty()){
            bufsize = x264.EncodeOneFrame(mat_img);
            if (bufsize > 0){
                buf = x264.GetEncodedFrame();
                printf("%s \n",(char *)buf);
            }
        }







        if(waitKey(33) == 27) break;
    }

    cvReleaseCapture(&capture);
    cvDestroyWindow("hebiao");
}



