// Homework 1
/*
    WHAT THIS CODE DO?
    1. input of image
    ------ HISTOGRAM EQUALIZATION PART ------
    2. save in a image the histogram of this image
    3. save in a image the histogram equalized color image 
    4. save the relative histogram of this equalized image (not necessary, only to see the obviusly different)
    5. save in a image the histogram equalized grayscale image
    ------ FILTERING PART ------
*/
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

void computeHist(Mat image, String output) {
    /*
        This function permits to calculate the histogram and print it, saving it into an img. First of all, it is necessary 
        to divide multi-channel array of the color space into separate single-channel array through split() function.
        Then it's necessary to calculate histogram of each one, through calcHist() function, and store value in the variables.
        After calculating histogram an image histImage is created to display the histogram which is created drawing 
        the line at each pixel for each channel.
    */
    
    const int histSize = 256;
    vector<Mat> bgr_planes;
    split(image, bgr_planes);

    float range[] = { 0, 256 };
    const float* histRange = { range };

    bool uniform = true;
    bool accumulate = false;

    Mat b_hist, g_hist, r_hist;

    calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);
    calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
    calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);
    int hist_w = 512;
    int hist_h = 400;
    int bin_w = cvRound((double)hist_w / histSize);

    Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

    normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
    normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
    normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());

    for (int i = 1; i < histSize; i++) {
        line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))), Point(bin_w * (i), hist_h - cvRound(b_hist.at<float>(i))), Scalar(255, 0, 0), 2, 8, 0);
        line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))), Point(bin_w * (i), hist_h - cvRound(g_hist.at<float>(i))), Scalar(0, 255, 0), 2, 8, 0);
        line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))), Point(bin_w * (i), hist_h - cvRound(r_hist.at<float>(i))), Scalar(0, 0, 255), 2, 8, 0);
    }

    imwrite(output, histImage);
}

int main(int argc, char** argv)
{
    //INPUT OF THE IMAGE
    Mat image = imread("original.png");

    // Check the image is a correct image
    if (image.empty()) {
        cout<<"error on image"<<endl;
        cin.get(); 
        return -1;
    }
    computeHist(image, "original_hist.jpg");

    // ------------- Histogram Equalization of a Color Image ------------- 
    /*
        First of all it is necessary to convert the loaded BGR image to a YCrCb image. 
        In fact, BGR color space does not permit to equalize the histogram without affecting to the color information 
        because all 3 channels contain color information. YCrCb color space permits a separation of the information: 
        only the Y channel of the image contains intensity information, so only that channel will be processed to get 
        a histogram equalized image without changing any color information.
        So, a vector<Mat> will be create to contain the different channels of the image with the split() function. 
        After the equalizaHist() function, where the parameters are the vector in [0] position (represent the Y channel), 
        it is necessary to merge all the channels in the vector to create the new color image.
        But, considering that this is image is in YCrCb color space, before saving the histogram equalized image, 
        it is necessary to convert it in the BGR color space with the cvtColor() function.
    */
    
    //PROCEDURE
    
    //STEP 1: Convert the image to YCrCb 
    Mat hist_equalized_image_color;
    cvtColor(image, hist_equalized_image_color, COLOR_BGR2YCrCb);

    //STEP 2: Create the vector and split the image into 3 channels (Y, Cr and Cb)
    vector<Mat> vec_channels;
    split(hist_equalized_image_color, vec_channels);

    //Equalize the histogram of only the Y channel 
    equalizeHist(vec_channels[0], vec_channels[0]);

    //Merge 3 channels in the vector to form the color image in YCrCB color space.
    merge(vec_channels, hist_equalized_image_color);

    //Convert the histogram equalized image from YCrCb to BGR color space again
    cvtColor(hist_equalized_image_color, hist_equalized_image_color, COLOR_YCrCb2BGR);

    //save the histogram equalized color image and save the histogram of this new image
    imwrite("equalized_color_image.jpg", hist_equalized_image_color);
    computeHist(hist_equalized_image_color, "equalized_color_image_hist.jpg");

    // ------------- Histogram Equalization of a Grayscale Image ------------- 
    /*
        The first step is to convert the loaded BGR image to a grayscale image throught the cvtColor() function.
        The procedure in this case is simpler. No vector<Mat> is needed because the histogram equalization works perfectly 
        in a grayscale image. So, it is only necessary the equalizeHist() function and then save the output image.
    */

    //PROCEDURE

    //change the color image to grayscale image
    cvtColor(image, image, COLOR_BGR2GRAY);

    //equalize the histogram
    Mat hist_equalized_image_bn;
    equalizeHist(image, hist_equalized_image_bn);

    //save the histogram equalized grayscale image
    imwrite("equalized_bn_image.jpg", hist_equalized_image_bn);

    waitKey(0); // Wait for any keystroke in any one of the windows
    destroyAllWindows(); //Destroy all opened windows
    return 0;
}
