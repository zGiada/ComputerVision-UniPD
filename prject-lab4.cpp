#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/stitching.hpp>
#include <iostream>
#include <string>

using namespace std;
using namespace cv;
string dir_path = "img1/"; //edit this to change directory -> img2

Mat load_img(string path) {
    Mat input = imread(path); //,0 -> grayscale
    return input;
}

int valSift = 0;
Mat sift_img_ret_descr(Mat img, int x, int y) {
    Ptr<SiftFeatureDetector> detector = SiftFeatureDetector::create();
    vector<KeyPoint> keypoints;
    Mat descriptors;
    detector->detectAndCompute(img, Mat(), keypoints, descriptors);
    Mat output;
    drawKeypoints(img, keypoints, output);
    string val = to_string(x) + to_string(y);
    string path_sifted_img = dir_path + "sifted_img/img_" + to_string(valSift) + "_sift.jpg";
    valSift++;
    imwrite(path_sifted_img, output);
    return descriptors;
}

vector<KeyPoint> sift_img_ret_keypoint(Mat img, int x, int y) {
    Ptr<SiftFeatureDetector> detector = SiftFeatureDetector::create();
    vector<KeyPoint> keypoints;
    Mat descriptors;
    detector->detectAndCompute(img, Mat(), keypoints, descriptors);
    Mat output;
    drawKeypoints(img, keypoints, output);
    return keypoints;
}

vector<Point2f> detect_location(vector<KeyPoint> k, std::vector<DMatch> v) {
    vector<Point2f> location;
    for (int i = 0; i < v.size(); i++) {
        location.push_back(k[v[i].queryIdx].pt);
    }
    return location;
}


int main()
{
    cout << "IMAGE STITCHING" << std::endl;    
    int i = 0;
    int j = 0;
    Mat img1, img2, sift_desc_1, sift_desc_2, img_complete_rows[3];
    vector<KeyPoint> sift_keyp_1, sift_keyp_2;
    string path = "", path2 = "";
    
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 2; j++) {
            if (j == 0) {
                path = dir_path +"patch/" + to_string(i) + to_string(j) + ".jpg";
                img1 = load_img(path);
                path2 = dir_path + "patch/" + to_string(i) + to_string(j + 1) + ".jpg";
                img2 = load_img(path2);
            }

            if (j == 1) {
                string val = to_string(i) + to_string(j - 1) + "-" + to_string(i) + to_string(j);
                path = dir_path + "partial_merge/merge_" + val + ".jpg"; 
                img1 = load_img(path);
                path2 = dir_path + "patch/" + to_string(i) + to_string(j + 1) + ".jpg";
                img2 = load_img(path2);
            }

            //2.
            sift_desc_1 = sift_img_ret_descr(img1, i, j);
            sift_desc_2 = sift_img_ret_descr(img2, i, j + 1);
            sift_keyp_1 = sift_img_ret_keypoint(img1, i, j);
            sift_keyp_2 = sift_img_ret_keypoint(img2, i, j + 1);

            //3.a
            Ptr<BFMatcher> matcher = BFMatcher::create(NORM_L2);
            vector<DMatch> matches, better_matches;
            matcher->match(sift_desc_1, sift_desc_2, matches);

            Mat img_matches;
            drawMatches(img1, sift_keyp_1, img2, sift_keyp_2, matches, img_matches);
            string name = dir_path+"bfMatcher/BFMatcher_img" + to_string(i) + to_string(j) + "-img" + to_string(i) + to_string(j + 1) + ".jpg";
            imwrite(name, img_matches);

            //3.b.
            double max = 0, min = 5000;
            int x;
            for (x = 0; x < matches.size(); x++) {
                double d = matches[x].distance;
                if (d < min)
                    min = d;
                if (d > max)
                    max = d;
            }
            for (x = 0; x < sift_desc_1.rows; x++) {
                if (matches[x].distance < 3 * min) {
                    better_matches.push_back(matches[x]);
                }
            }
            
            vector<Point2f> good_points_1, good_points_2;
            for (x = 0; x < better_matches.size(); x++) {
                good_points_1.push_back(sift_keyp_1[better_matches[x].queryIdx].pt);
                good_points_2.push_back(sift_keyp_2[better_matches[x].trainIdx].pt);
            }

            Mat homography = findHomography(good_points_2, good_points_1, RANSAC);
            Mat outwrap;
            warpPerspective(img2, outwrap, homography, Point(img1.cols + img2.cols, img1.rows));
            Mat half(outwrap, Rect(0, 0, img1.cols, img1.rows));
            img1.copyTo(half);

            if (j == 0) {
                path = dir_path+"partial_merge/merge_" + to_string(i) + to_string(j) + "-" + to_string(i) + to_string(j + 1) + ".jpg";
                imwrite(path, outwrap);
            }
            if (j == 1) {
                path = dir_path + "partial_merge/merge_row_" + to_string(i) + ".jpg";
                imwrite(path, outwrap);
            }

            img_complete_rows[i] = outwrap;
        }
    }

    for (i = 0; i < 3; i++) {
        imwrite(dir_path + "merge/img" + to_string(i) + ".jpg", img_complete_rows[i]);
    }
    for (i = 0; i < 2; i++) {
        if (i == 0) {
            img1 = load_img(dir_path + "merge/img" + to_string(i) + ".jpg");
            img2 = load_img(dir_path + "merge/img" + to_string(i + 1) + ".jpg");
        }
        else {
            img1 = load_img(dir_path + "merge/half_merge.jpg");
            img2 = load_img(dir_path + "merge/img" + to_string(i + 1) + ".jpg");
        }        
        rotate(img1, img1, ROTATE_90_COUNTERCLOCKWISE);
        rotate(img2, img2, ROTATE_90_COUNTERCLOCKWISE);
        sift_desc_1 = sift_img_ret_descr(img1, i, j);
        sift_desc_2 = sift_img_ret_descr(img2, i, j + 1);
        sift_keyp_1 = sift_img_ret_keypoint(img1, i, j);
        sift_keyp_2 = sift_img_ret_keypoint(img2, i, j + 1);
        Ptr<BFMatcher> matcher = BFMatcher::create(NORM_L2);
        vector<DMatch> matches, better_matches;
        matcher->match(sift_desc_1, sift_desc_2, matches);
        double max = 0, min = 5000;
        int x;
        for (x = 0; x < matches.size(); x++) {
            double d = matches[x].distance;
            if (d < min)
                min = d;
            if (d > max)
                max = d;
        }
        for (x = 0; x < sift_desc_1.rows; x++) {
            if (matches[x].distance < 3 * min) {
                better_matches.push_back(matches[x]);
            }
        }
        
        vector<Point2f> good_points_1, good_points_2;
        for (x = 0; x < better_matches.size(); x++) {
            good_points_1.push_back(sift_keyp_1[better_matches[x].queryIdx].pt);
            good_points_2.push_back(sift_keyp_2[better_matches[x].trainIdx].pt);
        }

        Mat homography = findHomography(good_points_2, good_points_1, RANSAC);
        Mat outwrap;
        warpPerspective(img2, outwrap, homography, Point(img1.cols + img2.cols, img1.rows));
        Mat half(outwrap, Rect(0, 0, img1.cols, img1.rows));
        img1.copyTo(half);
        if (i == 0) {
            imwrite(dir_path + "merge/half_merge.jpg", outwrap);
        }
        else {
            rotate(outwrap, outwrap, ROTATE_180);
            imwrite(dir_path + "merge_patch.jpg", outwrap);
        }
    }
    
    return 0;
}