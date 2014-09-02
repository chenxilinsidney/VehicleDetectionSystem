/**
 * @file camerashow.cpp
 * @brief vehicle detection system by OpenCV library
 * @author chenxilinsidney
 * @version 1.0
 * @date 2014-09-02
 */

#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

/// camera device index
int camera_device_index = 0;
/// video information
long totalFrameNumber = 0;
long frameToStart = 0;
long frameToStop = 0;
float rate = 0.0;
long currentFrame = 0;
float delay = 0.0;
bool findfirstline = false;
bool findsecondline = false;
float speed = 0.0;
/// camera image
Mat frame;
Mat capture_color_image;
Mat capture_gray_image;

/// vehicle detection system
Mat vehicle_detection_system(Mat& input_image);


/**
 * @brief main help
 */
void HelpMain(void)
{
    cout << "This demo: vehicle detection system by OpenCV library.\n";
}
/**
 * @brief select camera device
 */
void HelpSelectCamera(void)
{
    cout << "---please choose the camera device on your computer.---\n";
    cout << "warning:if you only one camera device on your computer,\n";
    cout << "please input the number 0 and press enter\n";
    cout << "if you have more than one camera device, you may input \n";
    cout << "other number as index(should smaller than the number of\n";
    cout << "camera devices on your computer).\n";
    cout << "if you want read the image sequense by video, please input"
        << "-1.\n";
    cout << endl;
    cout << "Input the index: ";
    cin >> camera_device_index;
    cout << "your choose is: " << camera_device_index << endl;
}

/**
 * @brief help for capture image and exit the program.
 */
void HelpCaptureImage(void)
{
    cout << "-warning: when press the key,please make sure the window named\n";
    cout << "-camera in foreground,or key would not response---------------\n";
    cout << "-----1.Press the \"S\" or \"s\" key to capture the camera. -\n";
    cout << "--------This will show both the captured gray and color images-\n";
    cout << "--------It will save the images named:color.bmp and gray.bmp  -\n";
    cout << "-----2.Press the \"Esc\" key to exit the program. -\n";
}
/**
 * @brief main function
 *
 * @return 0
 */
int main(void)
{
    HelpMain();

    HelpSelectCamera();
    
    VideoCapture cap;
    if (camera_device_index != -1) {
        /// open camera
        cap = VideoCapture(camera_device_index);
    } else {
        /// open video
        cout << "please input video name with pathname: ";
        String video_name;
        cin >> video_name;
        cout << "the video name is :" << video_name << endl;
        cap = VideoCapture(video_name);
    }
    if(!cap.isOpened()) {
        cout << "Can not find the default camera from you computer!\n";
        cin.get();
        return -1;
    }

    /// wait for camera to get ready
    waitKey(2000);

    /// video information
    totalFrameNumber = cap.get(CV_CAP_PROP_FRAME_COUNT);
    frameToStart = 30;
    frameToStop = 140;
    rate = cap.get(CV_CAP_PROP_FPS);
    currentFrame = frameToStart;
    delay = 1000/rate;
    if (camera_device_index == -1) {
        cap.set( CV_CAP_PROP_POS_FRAMES,frameToStart);
    }
    /// read a frame to get the camera image state
    cap.read(frame);
    resize(frame, frame, Size(640,480));
    cout << "image height = " << frame.rows << endl;
    cout << "image width = " << frame.cols << endl;
    cout << "image channel = " << frame.channels() << endl;
    imshow("camera", frame);
    cout << "camera/video open success\n";
    waitKey(30);

    HelpCaptureImage();

    /// start show the camera
    char key = -1;
    for(;;) {
        Mat frame;
        cap.read(frame);
        frame = vehicle_detection_system(frame);
        currentFrame++;
        imshow("camera", frame);
        key = waitKey(30);
        /// exit programe
        if(key == 27) {
            destroyAllWindows();
            break;
        }
        /// capture image
        if(key == 'S' || key == 's') {
            capture_color_image = frame.clone();
            cvtColor(capture_color_image, capture_gray_image,
                    cv::COLOR_BGR2GRAY);
            // imshow("color", capture_color_image);
            // imshow("gray", capture_gray_image);
            // imwrite("color.bmp", capture_color_image);
            // imwrite("gray.bmp", capture_gray_image);
        }
    }
    cap.release();
    return 0;
}

/// vehicle detection system
Mat vehicle_detection_system(Mat& input_image)
{
    Mat output_image = input_image.clone();
    Mat gray_image = input_image.clone();
    Mat canny_image = input_image.clone();
    GaussianBlur(gray_image, canny_image, Size(17, 17), 2, 2);
    Canny(canny_image, canny_image, 50, 200, 3);
    vector<Vec4i> lines;
    HoughLinesP(canny_image, lines, 1, CV_PI/180, 400, 200, 100);

    int num = 0;
    float BaseDis = 0;

    for (size_t i = 0; i < lines.size() ; i++)
    {
        float x1 = float(lines[i][0]);
        float y1 = float(lines[i][1]);
        float x2 = float(lines[i][2]);
        float y2 = float(lines[i][3]);

        float a = (y2-y1)/(x2-x1);
        float theta = atan(a);
        theta = theta*180/CV_PI;

        if (lines[i][1]>350 && lines[i][3]>350)
        {
            if (theta>-15 && theta<-5)
            {
                float A = y1-y2;
                float B = x2-x1;
                float C = x1*y2-x2*y1;
                float d = abs(C)/sqrt(A*A+B*B);
                // printf("line %f %f\n", d, BaseDis);
                if (0==num)
                {
                    BaseDis = d;
                    Point pt1=Point(lines[i][0],lines[i][1]);
                    Point pt2=Point(lines[i][2],lines[i][3]);
                    line(output_image,pt1,pt2,Scalar(0,0,255),2,8);
                    // printf("line 1 distance = %f\n", d);
                    num++;
                } 
                else
                {
                    //找到第二条停止线的边缘霍夫线,并以红线标出
                    if ((abs(d-BaseDis))>5)
                    {
                        Point pt1=Point(lines[i][0],lines[i][1]);
                        Point pt2=Point(lines[i][2],lines[i][3]);
                        line(output_image,pt1,pt2,Scalar(0,0,255),2,8);
                        // printf("line 2 distance = %f\n", d);
                        num++;
                        break;
                    }
                }				
            }			
        }
    }
    // printf("line = %d %d\n", findfirstline, findsecondline);
    // printf("num = %d\n", num);
    if (!(findfirstline && findsecondline)) {
        //记录第一条停止霍夫线灰度突变的帧位置并显示
        if (1==num) {
            long FirstHitFrameNumber = currentFrame;
            findfirstline = true;
            printf("first frame number = %ld\n", FirstHitFrameNumber);
        }
        //记录第二条停止霍夫线灰度突变的帧位置并显示
        if (0==num) {
            long SecongHitFrameNumber = currentFrame;
            printf("second frame number = %ld\n", SecongHitFrameNumber);
            findsecondline = true;
            speed = 0.2/float(delay)*1000*3.6;
            printf("speed = %f\n", speed);
        }
    }
    // resize(canny_image, canny_image, Size(640,480));
    // return canny_image;
    resize(output_image, output_image, Size(640,480));
    return output_image;
}

