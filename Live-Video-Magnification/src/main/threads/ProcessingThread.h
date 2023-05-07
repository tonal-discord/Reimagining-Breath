/************************************************************************************/
/* An OpenCV/Qt based realtime application to magnify motion and color              */
/* Copyright (C) 2015  Jens Schindel <kontakt@jens-schindel.de>                     */
/*                                                                                  */
/* Based on the work of                                                             */
/*      Joseph Pan      <https://github.com/wzpan/QtEVM>                            */
/*      Nick D'Ademo    <https://github.com/nickdademo/qt-opencv-multithreaded>     */
/*                                                                                  */
/* Realtime-Video-Magnification->ProcessingThread.h                                 */
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

#ifndef PROCESSINGTHREAD_H
#define PROCESSINGTHREAD_H

// C++
#include <cmath>
#include <stdio.h>
#include <iostream>

#ifdef __linux__
// Linux
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include <sys/types.h>
    #include <sys/ipc.h>
    #include <sys/shm.h>
#elif _WIN32
// Windows
    #define BUF_SIZE 256
    #include <windows.h>
    #include <WinNT.h>
    #include <conio.h>
    #include <tchar.h>
    #include <memoryapi.h>
    #include <handleapi.h>
#else
#endif


#ifdef __linux__
// Linux

#elif _WIN32
// Windows

#else
#endif

// Qt
#include <QtCore/QThread>
#include <QtCore/QTime>
#include <QtCore/QQueue>
#include "QDebug"
// OpenCV
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
// Local
#include "main/other/Structures.h"
#include "main/other/Config.h"
#include "main/other/Buffer.h"
#include "main/helper/MatToQImage.h"
#include "main/helper/SharedImageBuffer.h"
#include "main/magnification/Magnificator.h"

//using namespace cv;

class ProcessingThread : public QThread
{
    Q_OBJECT

    public:
        ProcessingThread(SharedImageBuffer *sharedImageBuffer, int deviceNumber);
        ~ProcessingThread();
        bool releaseCapture();
        QRect getCurrentROI();
        void stop();
        void getOriginalFrame(bool doEmit);
        bool startRecord(std::string filepath, bool captureOriginal);
        void stopRecord();
        bool isRecording();
        int getFPS();
        int getRecordFPS();
        int savingCodec;

    private:
        void updateFPS(int);
        bool processingBufferFilled();
        void fillProcessingBuffer();
        Magnificator magnificator;
        SharedImageBuffer *sharedImageBuffer;
        cv::Mat currentFrame;
        int temp;
        cv::Mat combinedFrame;
        cv::Mat originalFrame;
        cv::Rect currentROI;
        QImage frame;
        QElapsedTimer t;
        QQueue<int> fps;
        QMutex doStopMutex;
        QMutex processingMutex;
        cv::Size frameSize;
        std::vector<cv::Mat> processingBuffer;
        int processingBufferLength;
        cv::Point framePoint;
        struct ImageProcessingFlags imgProcFlags;
        struct ImageProcessingSettings imgProcSettings;
        struct ThreadStatisticsData statsData;
        volatile bool doStop;
        int processingTime;
        int fpsSum;
        int sampleNumber;
        int deviceNumber;
        bool emitOriginal;
        bool doRecord;
        cv::VideoWriter output;
        int framesWritten;
        int recordingFramerate;
        bool captureOriginal;
        cv::Mat combineFrames(cv::Mat &frame1, cv::Mat &frame2);
        QMutex recordMutex;
        int frameNum;
        int prevFrameNum = 0;
        int breathValues[3];
        float prevSumm = 0;
        bool CSV;

    protected:
        void run();

    public slots:
        void updateImageProcessingFlags(struct ImageProcessingFlags);
        void updateImageProcessingSettings(struct ImageProcessingSettings);
        void setROI(QRect roi);
        void updateFramerate(double fps);

    signals:
        void newFrame(const QImage &frame);
        void origFrame(const QImage &frame);
        void updateStatisticsInGUI(struct ThreadStatisticsData);
        void sendNumFrames(int numFrames);
        void frameWritten(int frames);
        void maxLevels(int levels);
};

#endif // PROCESSINGTHREAD_H
