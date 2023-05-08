/************************************************************************************/
/* An OpenCV/Qt based realtime application to magnify motion and color              */
/* Copyright (C) 2015  Jens Schindel <kontakt@jens-schindel.de>                     */
/*                                                                                  */
/* Based on the work of                                                             */
/*      Joseph Pan      <https://github.com/wzpan/QtEVM>                            */
/*      Nick D'Ademo    <https://github.com/nickdademo/qt-opencv-multithreaded>     */
/*                                                                                  */
/* Realtime-Video-Magnification->Magnificator.h                                     */
/*                                                                                  */
/* This program is free software: you can redistribute it and/or modify             */
/* it under the terms of the GNU General Public License as published by             */
/* the Free Software Foundation, either version 3 of the License, or                */
/* (at your option) any later version.                                              */
/*                                                                                  */
/* This program is distributed in the hope that it will be useful,                  */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of                   */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    */
/* GNU General Public License for more details.                                     */
/*                                                                                  */
/* You should have received a copy of the GNU General Public License                */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>.            */
/************************************************************************************/

#ifndef MAGNIFICATOR_H
#define MAGNIFICATOR_H
// Qt
#include "QList"
#include "QTime"
#include "QFile"
#include <qdebug.h>
// OpenCV
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
// Local
#include "main/magnification/SpatialFilter.h"
#include "main/magnification/TemporalFilter.h"
#include "main/other/Structures.h"
#include "main/other/Config.h"
#include "main/magnification/RieszPyramid.h"
// C++
#include "cmath"
#include "math.h"

#define M_PI 3.14159265358979323846264338327950288

//using namespace cv;
using namespace std;
/*!
 * \brief The Magnificator class Handles the motion and color magnification. The class also holds
 *  a Buffer with magnified images and variables describing the inner status of the magnification
 *  process, like the current level we are processing in our image pyramid.
 */
class Magnificator
{
public:
    ////////////////////////
    ///Magnification ///////
    ////////////////////////
    /*!
     * \brief Magnificator Constructor for this class.
     * \param pBuffer Pointer to a processing buffer (preferably with
     *  - colorMagnification: the size of a power of 2 , near twice the framerate
     *  - motionMagnification: the size 2)
     *  of the video that is magnified.
     * \param imageProcFlags Pointer to the flags, yet only used to indicate using grayscale images
     * \param imageProcSettings Pointer to the Settings for magnification. These settings can be changed during
     *  the magnification process
     */
    Magnificator(vector<cv::Mat> *pBuffer = 0,
                 struct ImageProcessingFlags *imageProcFlags = 0,
                 struct ImageProcessingSettings *imageProcSettings = 0,
                 int *numFrames = 0);
    ~Magnificator();
    /*!
     * \brief calculateMaxLevels Maximum levels an image pyramid can hold.
     * \return Maximum level.
     */
    int calculateMaxLevels();
    int calculateMaxLevels(cv::Size s);
    int calculateMaxLevels(QRect r);

    /*!
     * \brief laplaceMagnify Motion magnification. You can find detailed step by step description in .cpp
     */
    void laplaceMagnify();
    /*!
     * \brief colorMagnify Color magnification. You can find detailed step by step description in .cpp
     */
    void colorMagnify();
    /*!
     * \brief waveletMagnify Haar Wavelet magnification. You can find detailed step by step description in .cpp
     */
    void rieszMagnify();

    ////////////////////////
    ///Magnified Buffer ///
    ////////////////////////
    /*!
     * \brief getFrameLast Returns the last/newest magnified image. After that, the first/oldest frame is deleted.
     * \return A cv::Mat with the same size like the images in the provided processingBuffer.
     */
    cv::Mat getFrameLast();
    /*!
     * \brief getFrameFirst Returns the first/oldest magnified image. After that, this frame will be deleted.
     * \return A cv::Mat with the same size like the images in the provided processingBuffer.
     */
    cv::Mat getFrameFirst();
    /*!
     * \brief getFrameAt Returns a magnified image on a specified position in the internal buffer.
     * \param n Position with 0 <= n < buffersize.
     * \return A cv::Mat with the same size like the images in the provided processingBuffer.
     */
    cv::Mat getFrameAt(int n);
    /*!
     * \brief getBufferSize Size of the internal Buffer, holding the magnified images.
     * \return Int with Size < processingBuffer that is provided.
     */
    int getBufferSize();
    /*!
     * \brief clearBuffer Cleans buffer with magnified images, deletes lowpass pyramids, temporal buffer,
     */
    void clearBuffer();

    bool hasFrame();

    ////////////////////////
    ///Processing Buffer //
    ////////////////////////
    /*!
     * \brief getOptimalBufferSize Used to calculate buffer and cache size for color magnification.
     *  Best results with ~2 seconds film material and a size that is a power of 2.
     * \param fps Framerate to calculate how many frames are 2 seconds of film material.
     * \return Int, power of 2, minimum 16.
     */
    int getOptimalBufferSize(int fps);

    int breathMeasureOutput;

private:
    /*!
     * \brief processingBuffer Pointer to processing buffer, given in constructor. Holds images that have to
     *  be processed (important for playing videos). Is either 2 or a power of two (see getOptimalBufferSize())
     */
    vector<cv::Mat> *processingBuffer;

    ////////////////////////
    ///External Settings //
    ////////////////////////
    /*!
     * \brief imgProcFlags Pointer to processing flags, given in constructor. Effectively used to
     *  indicate grayscale mode.
     */
    ImageProcessingFlags *imgProcFlags;
    /*!
     * \brief imgProcSettings Pointer to processing settings, given in constructor. Can change dynamically
     *  while runtime.
     */
    ImageProcessingSettings *imgProcSettings;

    int *numFrames;

    ////////////////////////
    ///Internal Settings //
    ////////////////////////
    /*!
     * \brief delta (Motion magnification) Calculated by cutoff wavelength and amplification.
     */
    float delta;
    /*!
     * \brief exaggeration_factor (Motion magnification) Amplification booster for better
     *  visualization.
     */
    float exaggeration_factor;
    /*!
     * \brief lambda (Motion magnification) Representative wavelength. Is reduced for every
     *  image pyramid level.
     */
    float lambda;
    /*!
     * \brief currentFrame Current image that is written in magnified buffer.
     *  0 <= currentFrame < magnifiedBuffer.size().
     */
    int currentFrame;
    /*!
     * \brief levels Number of levels for Laplace/Gauss Pyramid.
     */
    int levels;

    ////////////////////////
    ///Cache ///////// ///////
    ////////////////////////
    /*!
     * \brief magnifiedBuffer (Both) Holds magnified images, that are not yet given to the GUI.
     */
    vector<cv::Mat> magnifiedBuffer;
    /*!
     * \brief tempBuffer (Motion magnification) Holds image pyramid with the difference of two
     *  filtered images from lowpassHi & lowpassLo on each level. The upsampled pyramid is a motion
     *  image that will be added to the original image.
     */
    vector<cv::Mat> motionPyramid;
    /*!
     * \brief lowpassHi (Motion magnification) Holds image pyramid of lowpassed current frame with
     *  high cutoff
     */
    vector<cv::Mat> lowpassHi;
    /*!
     * \brief lowpassLo (Motion magnification) Holds image pyramid of lowpassed current frame with
     *  low cutoff
     */
    cv::Mat prevFrame;
    /*!
     * \brief prevFrame (Motion magnification) Holds previous frame to detect motion.
     */

    vector<cv::Mat> lowpassLo;
    /*!
     * \brief downSampledMat (Color magnification) Holds 2*fps rounded to next power of 2
     *  downsampled and to 1 column reshaped images.
     */
    cv::Mat downSampledMat;

    std::shared_ptr<RieszPyramid> oldPyr;
    std::shared_ptr<RieszPyramid> curPyr;
    std::shared_ptr<RieszTemporalFilter> loCutoff;
    std::shared_ptr<RieszTemporalFilter> hiCutoff;

    ////////////////////////
    ///Postprocessing //////
    ////////////////////////
    /*!
     * \brief amplifyLaplacian (Motion magnification) Amplifies a Laplacian image pyramid.
     * \param src Source image.
     * \param dst Destination image.
     * \param currentLevel Level of image pyramid that is amplified.
     */
    void amplifyLaplacian(const cv::Mat &src, cv::Mat &dst, int currentLevel);
    /*!
     * \brief attenuate (Motion magnification) Attenuates the 2 last channels of a Lab-image.
     * \param src Source image.
     * \param dst Destination image.
     */
    void attenuate(const cv::Mat &src, cv::Mat &dst);
    /*!
     * \brief amplifyGaussian (Color magnification) Amplifies a Gaussian image pyramid.
     * \param src Source image.
     * \param dst Amplified image.
     */
    void amplifyGaussian(const cv::Mat &src, cv::Mat &dst);

};

#endif // MAGNIFICATOR_H
