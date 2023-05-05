/************************************************************************************/
/* An OpenCV/Qt based realtime application to magnify motion and color              */
/* Copyright (C) 2015  Jens Schindel <kontakt@jens-schindel.de>                     */
/*                                                                                  */
/* Based on the work of                                                             */
/*      Joseph Pan      <https://github.com/wzpan/QtEVM>                            */
/*      Nick D'Ademo    <https://github.com/nickdademo/qt-opencv-multithreaded>     */
/*                                                                                  */
/* Realtime-Video-Magnification->PlayerThread.h                                     */
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

#ifndef PLAYERTHREAD_H
#define PLAYERTHREAD_H
#define BUF_SIZE 256

// C++
#include <cmath>
// Windows/C++
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <memoryapi.h>
#include <handleapi.h>
#include <iostream>
#include <WinNT.h>

// Qt
#include <QtCore/QThread>
#include <QFile>
#include <QMutex>
#include <QDebug>
#include <QtCore/QTime>
#include <QtCore/QQueue>
// OpenCV
#include <opencv2/highgui/highgui.hpp>
// Local
#include "main/other/Config.h"
#include "main/other/Structures.h"
#include "main/helper/MatToQImage.h"
#include "main/magnification/Magnificator.h"

// using namespace cv;

class PlayerThread : public QThread
{
    Q_OBJECT
    
    public:
        PlayerThread(const std::string filepath, int width, int height, double fps);
        ~PlayerThread();
        void stop();
        // Capture
        bool loadFile();
        bool releaseFile();
        bool isFileLoaded();
        int getInputSourceWidth();
        int getInputSourceHeight();
        // Process
        QRect getCurrentROI();
        // Player
        bool isStopping();
        bool isPausing();
        bool isPlaying();
        void setCurrentFrame(int framenumber);
        void playAction();
        void stopAction();
        void pauseAction();
        void setCurrentTime(int ms);
        double getCurrentFramenumber();
        double getCurrentPosition();
        double getInputFrameLength();
        double getInputTimeLength();
        double getFPS();
        void getOriginalFrame(bool doEmit);

private:
        QMutex doStopMutex;
        struct ThreadStatisticsData statsData;
        double lengthInMs;
        double lengthInFrames;
        const std::string filepath;
        int getCurrentReadIndex();
        // Capture
        cv::VideoCapture cap;
        cv::Mat grabbedFrame;
        int playedTime;
        int width;
        int height;
        std::vector<cv::Mat> originalBuffer;
        void setBufferSize();
        // Process
        // This is the current Framenr that is grabbed from magnificator is always
        // < processingBufferLength, used to process videos rightly from beginning of vid
        int currentWriteIndex;
        double fps;
        void updateFPS(int timeElapsed);
        cv::Mat currentFrame;
        cv::Rect currentROI;
        QImage frame;
        QImage originalFrame;
        // processing measurement
        QElapsedTimer t;
        int processingTime;
        int fpsSum;
        int sampleNumber;
        QQueue<int> fpsQueue;
        QMutex processingMutex;
        cv::Size frameSize;
        cv::Point framePoint;
        struct ImageProcessingFlags imgProcFlags;
        struct ImageProcessingSettings imgProcSettings;
        // Player variables
        volatile bool doStop;
        volatile bool doPause;
        volatile bool doPlay;
        bool emitOriginal;
        void endOfFrame_action();
        // Magnifying
        bool processingBufferFilled();
        void fillProcessingBuffer();
        Magnificator magnificator;
        std::vector<cv::Mat> processingBuffer;
        int processingBufferLength;
        int frameNum = 0;
        int prevFrameNum = 0;
        int breathValues[3];
        float prevSumm = 0;
        bool CSV;


protected:
        void run();

private slots:
        void updateImageProcessingFlags(struct ImageProcessingFlags);
        void updateImageProcessingSettings(struct ImageProcessingSettings);
        void setROI(QRect roi);
        void pauseThread();

signals:
        void updateStatisticsInGUI(struct ThreadStatisticsData);
        void newFrame(const QImage &frame);
        void origFrame(const QImage &frame);
        void endOfFrame();
        void maxLevels(int levels);
};

#endif // PLAYERTHREAD_H
