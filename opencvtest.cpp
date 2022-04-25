// Homework 1
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
    //INPUT OF THE IMAGE
    Mat image = imread("original.png");

    // Check the image is a correct image
    if (image.empty())
    {
        cout << "Could not open or find the image" << endl;
        cin.get(); 
        return -1;
    }

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

    //save the histogram equalized color image
    imwrite("equalized_color_image.jpg", hist_equalized_image_color);

    


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
