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

void udp_send_yuv_img();
void udp_send_rgb_img();
void udp_recive_data();

char ** split_buffer(char *pYuvBuf,unsigned long max_length,unsigned long * arr_size){


    unsigned long total =  strlen(pYuvBuf);

    unsigned long count = total/max_length +(total%max_length==0?0:1);

    *arr_size = count;

    unsigned long tail_length = total - (count-1)*max_length;

    char **c_list =(char **)calloc(count, sizeof(char *));

    for (int kk = 0; kk<count; kk++) {
        if (kk == count-1) {
            c_list[kk] = (char *)calloc(tail_length,sizeof(char));
        }else{
            c_list[kk] = (char *)calloc(max_length,sizeof(char));
        }

    }

    for (int i=0; i<count; i++) {
        if (i == count -1) {
            memcpy(c_list[i],pYuvBuf +i*max_length, tail_length);
        }else{
            memcpy(c_list[i],pYuvBuf +i*max_length, max_length);
        }
    }

    return c_list;
}






int main() {



    udp_recive_data();

    return 1;
}



void udp_recive_data(){

    struct sockaddr_in si_me, si_other;
    int s;
    socklen_t slen =  sizeof(si_other);
    char buf[BUFLEN];
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

        if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr*)(&si_other), &slen) != -1 ){

            printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
//            c_.host = si_other.sin_addr.s_addr;
//            c_.port = si_other.sin_port;

            printf("%s",buf);
        }

        if (s == -999){
            break;
        }


    }
}

void udp_send_yuv_img(){


    CvCapture *capture = cvCreateCameraCapture(0);
    IplImage *frame;



    int frameH    = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
    int frameW    = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);



    printf("%d,%d",frameW,frameH);

    int yuv_bufLen = frameW*frameH*3/2;
    unsigned char* pYuvBuf = new unsigned char[yuv_bufLen];



    struct sockaddr_in si_me, si_other;
    int s;
    socklen_t slen =  sizeof(si_other);
    char buf[BUFLEN];
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


    //    int max_str_len = 9216;
    int  send_max_data_length =  500;
    while (1) {
        frame = cvQueryFrame(capture);
        Mat mat_img = cvarrToMat(frame);

        Mat yuvImg;
        cvtColor(mat_img, yuvImg, CV_BGR2YUV_I420);
        memcpy(pYuvBuf, yuvImg.data, yuv_bufLen*sizeof(unsigned char));



        si_other.sin_addr.s_addr = c_.host;
        si_other.sin_port = c_.port;

        printf("Sending to %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));

        printf("============== %d",yuv_bufLen);
//        1382400

        unsigned long arr_size ;
        char  ** arr_buffer = split_buffer((char *)pYuvBuf,send_max_data_length,&arr_size);


        char  *start = "start";
        if (sendto(s, start, strlen(start), 0, (struct sockaddr*)(&si_other), slen)==-1)
            diep("sendto");
        for (int k = 0; k < arr_size; ++k) {
            char *m_chr = arr_buffer[k];
            if (sendto(s, m_chr, strlen(m_chr), 0, (struct sockaddr*)(&si_other), slen)==-1)
                diep("sendto");
        }

        char  *end = "end";
        if (sendto(s, end, strlen(end), 0, (struct sockaddr*)(&si_other), slen)==-1)
            diep("sendto");


        if(waitKey(1) == 27) break;

    }





}



void udp_send_rgb_img(){

    CvCapture *capture = cvCreateCameraCapture(0);
    IplImage *frame;



    int frameH    = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
    int frameW    = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);



    printf("%d,%d",frameW,frameH);

    int yuv_bufLen = frameW*frameH*3/2;
    unsigned char* pYuvBuf = new unsigned char[yuv_bufLen];



    struct sockaddr_in si_me, si_other;
    int s;
    socklen_t slen =  sizeof(si_other);
    char buf[BUFLEN];
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


//    int max_str_len = 9216;
    int  send_max_data_length =  500;
    while (1) {
        frame = cvQueryFrame(capture);
        Mat mat_img = cvarrToMat(frame);



        memcpy(pYuvBuf, mat_img.data, yuv_bufLen*sizeof(unsigned char));



        si_other.sin_addr.s_addr = c_.host;
        si_other.sin_port = c_.port;

        printf("Sending to %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));

        printf("============== %d",yuv_bufLen);
//      1382400

        unsigned long arr_size ;
        char  ** arr_buffer = split_buffer((char *)pYuvBuf,send_max_data_length,&arr_size);


        char  *start = "start";
        if (sendto(s, start, strlen(start), 0, (struct sockaddr*)(&si_other), slen)==-1)
            diep("sendto");
        for (int k = 0; k < arr_size; ++k) {
            char *m_chr = arr_buffer[k];
            if (sendto(s, m_chr, strlen(m_chr), 0, (struct sockaddr*)(&si_other), slen)==-1)
                diep("sendto");
        }

        char  *end = "end";
        if (sendto(s, end, strlen(end), 0, (struct sockaddr*)(&si_other), slen)==-1)
            diep("sendto");


        if(waitKey(125) == 27) break;

    }


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



