// Homework 1 - Zuccolo giada - matr. 2055702
/*
    WHAT THIS CODE DO?
    1. input of image called "images/original/original.png"
    ------ HISTOGRAM EQUALIZATION PART ------
    2. save in a image the histogram of this image
    3. save in a image the histogram equalized color image
    4. save the relative histogram of this equalized image (not necessary, only to see the obviusly different)
    5. save in a image the histogram equalized grayscale image
    
    ------ FILTER PART ------
    6. Insert values of all filter
        6.1.    Insert value for bilateral filtering -- RANGE 
        6.2.    Insert value for bilateral filtering -- SPACE
        6.2.    Insert value for gaussian blur -- kernel
        6.2.    Insert value for gaussian blur -- sigma
        6.2.    Insert value for median
    7. All the filtered images will be saved in the correspondent directory

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

// ------------------------------------ MAIN ------------------------------------ 

int main(int argc, char** argv)
{
    //INPUT OF THE IMAGE for histogram equalization
    Mat image = imread("images/original/original_histogram.jpg");

    // Check the image is a correct image
    if (image.empty()) {
        cout << "error on image" << endl;
        cin.get();
        return -1;
    }
    computeHist(image, "images/original/original_histogram_hist.jpg");

    //  ------ HISTOGRAM EQUALIZATION PART ------
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
    imwrite("images/histogram_equalized/hist_equalized_color_image.jpg", hist_equalized_image_color);
    computeHist(hist_equalized_image_color, "images/histogram_equalized/equalized_color_image_hist.jpg");
    //save the same image but with an luminance equalization
    cv::Mat image_lum = image.clone();
    cv::Mat tmp_img;
    cv::cvtColor(image_lum, tmp_img, 44);
    std::vector<cv::Mat> luminance;
    cv::split(tmp_img, luminance);
    cv::equalizeHist(luminance[0], luminance[0]);
    cv::merge(luminance, tmp_img);
    cv::cvtColor(tmp_img, image_lum, 56);
    imwrite("images/histogram_equalized/hist_equalized_luminance_color_image.jpg", image_lum);

    // ------------- Histogram Equalization of a Grayscale Image ------------- 
    /*
        The first step is to convert the loaded BGR image to a grayscale image throught the cvtColor() function.
        The procedure in this case is simpler. No vector<Mat> is needed because the histogram equalization works perfectly
        in a grayscale image. So, it is only necessary the equalizeHist() function and then save the output image.
    */

    //PROCEDURE

    //change the color image to grayscale image
    cv::Mat image_bn = image.clone();
    cv::Mat tmp_img_bn;
    cvtColor(image_bn, tmp_img_bn, COLOR_BGR2GRAY);

    //equalize the histogram
    Mat hist_equalized_image_bn;
    equalizeHist(tmp_img_bn, hist_equalized_image_bn);

    //save the histogram equalized grayscale image
    imwrite("images/histogram_equalized/hist_equalized_bn_image.jpg", hist_equalized_image_bn);




    //  ------ FILTER PART ------
    
    //INPUT OF THE IMAGE for filter part
    image = imread("images/original/original_filter.jpg");

    // Check the image is a correct image
    if (image.empty()) {
        cout << "error on image" << endl;
        cin.get();
        return -1;
    }

    //insert ALL the values
    
    //BILATERAL FILTER -> range => size, space value, kernel size:     
    cout << "\n --- FILTERING PART ---\n Entering ACCETTABLE values\n >> BILATERAL FILTER: RANGE bilateral\n\tSet the value:" << endl;
    cout<<"\t\tSize (integer) = ";
    double sizeBR;
    cin>> sizeBR;
    cout << "\t\tKernel size (double) = ";
    double kernelBR;
    cin >> kernelBR;
    if ((int)(kernelBR) % 2 == 0)
        kernelBR = kernelBR - 1;
    cout << "\t\tSpace value (double) = ";
    double space;
    cin >> space;
    if ((int)(space) % 2 == 0)
        space = space - 1;

    //BILATERAL FILTER -> space => size, space value, kernel size: 
    cout << "\n>> BILATERAL FILTER: SPACE bilateral\n\tSet the value:" << endl;
    cout << "\t\tSize (integer) = ";
    double sizeBS;
    cin >> sizeBS;
    cout << "\t\tRange value (double) = ";
    double range;
    cin >> range;
    if ((int)(range) % 2 == 0)
        range = range - 1;
    cout << "\t\tKernel size (double) = ";
    double kernelBS;
    cin >> kernelBS;
    if ((int)(kernelBS) % 2 == 0)
        kernelBS = kernelBS - 1;
     
    //BILATERAL FILTER -> space => size, space value, kernel size: 
    cout << "\n>> MEDIAN FILTER\n\tSet the value:" << endl;
    cout << "\t\tKernel size (double) = ";
    double kernelM;
    cin >> kernelM;
    if ((int)(kernelM) % 2 == 0)
        kernelM = kernelM - 1;

    //GAUSSIAN FILTER -> kernel => size(value,value), sigmaX, sigmaY
    cout << "\n>> GAUSSIAN FILTER: KERNAL gaussian\n\tSet the value:" << endl;
    cout << "\t\tSize (width) = ";
    int wdt;
    cin >> wdt;
    if (wdt % 2 == 0)
        wdt--;
    cout << "\t\tSize (height) = ";
    int hgt;
    cin >> hgt;
    if (hgt % 2 == 0)
        hgt--;
    Size sizeGB(wdt, hgt);
    cout << "\t\tThe value of size will used for sigmaX and sigmaY" << endl;
    cout << "\n>> GAUSSIAN FILTER: SIGMA gaussian\n\tSet the value:\n\t\tSigma (double) = ";
    double sigma;
    cin >> sigma;
   
    
    //imshow("Original image", image);
    //save images
    Mat img_bil_range = image.clone();
    Mat bilater_range_filter_img;
    bilateralFilter(img_bil_range, bilater_range_filter_img, sizeBR, kernelBR, space);
    ///imshow("Range bilateral", bilater_range_filter_img);
    imwrite("images/filtering/bilateral_filter/range_bilateral.jpg", bilater_range_filter_img);

    Mat img_bil_spac = image.clone();
    Mat bilater_space_filter_img;
    bilateralFilter(img_bil_spac, bilater_space_filter_img, sizeBS, range, kernelBS);
    imwrite("images/filtering/bilateral_filter/space_bilateral.jpg", bilater_space_filter_img);
    //imshow("Space bilateral", bilater_space_filter_img);
   
    
    Mat img_gauss_kern = image.clone();
    Mat gauss_kern;
    GaussianBlur(img_gauss_kern, gauss_kern, sizeGB, 0);
    imwrite("images/filtering/gaussian_filter/kernel_filter.jpg", gauss_kern);
    //imshow("kernel", gauss_kern);

    Mat img_gauss_sigma = image.clone();
    Mat gauss_sigma;
    GaussianBlur(img_gauss_sigma, gauss_sigma, sizeGB, (double)sigma, 0);
    imwrite("images/filtering/gaussian_filter/sigma_filter.jpg", gauss_sigma);
    //imshow("gauss_sigma", gauss_sigma);

    Mat img_median = image.clone();
    Mat median_filter;
    medianBlur(img_median, median_filter, kernelM);
    imwrite("images/filtering/median_filter/median_filter.jpg", median_filter);
    //imshow("median", median_filter);

    waitKey(0); // Wait for any keystroke in any one of the windows
    destroyAllWindows(); //Destroy all opened windows
    return 0;
}
