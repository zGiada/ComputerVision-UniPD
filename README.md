# COMPUTER VISION LAB 2 => HOMEWORK 1

> **Topics**: Image Equalization, Histograms, Image Filtering

## Part 1: Histogram Equalization

Write a program that:

1. Loads an image 
2. Prints the histograms of the image. You must compute 3 histograms, one for each channel (i.e., R, G and B) with 256 bins and [0, 255] as range. Notice that you need to use the `calcHist()` function separately on the 3 channels. You can use the provided function (in the `show_histogram_function.cpp` file) to visualize the data. 
3. Equalizes separately the R, G and B channels by using `cv::equalizeHist()`. 
4. Shows the equalized image and the histogram of its channels. 
5. Notice the artifacts produced by this approach. To obtain a better equalization than the one of point 4, convert the image to a different color space, e.g. Lab (use `cv::cvtColor()` with `COLOR_BGR2Lab` as color space conversion code), and equalize only the luminance (L) channel.

## Part 2: Image Filtering

Generate a denoised version of the image. You should try different filters and parameter values. 

- Write a program that performs the filtering and shows the result. 

- Requested filters to test and parameters to be set for each filter:

  | FILTERS                 | PARAMETERS                                                   |
  | ----------------------- | ------------------------------------------------------------ |
  | `cv::medianFilter()`    | kernel_size                                                  |
  | `cv::GaussianBlur()`    | - kernel_size (keep it square)<br />- σ : assume σx = σy     |
  | `cv::bilateralFilter()` | - kernel_size (you can use a fixed value or use the 6σs rule) <br />- sigma_range σr <br />- sigma_space σs |

- You can simply pass the filter parameters from the command line *or (*advanced solution, optional**) use some trackbars*