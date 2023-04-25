/************************************************************************************/
/* An OpenCV/Qt based realtime application to magnify motion and color              */
/* Copyright (C) 2015  Jens Schindel <kontakt@jens-schindel.de>                     */
/*                                                                                  */
/* Based on the work of                                                             */
/*      Joseph Pan      <https://github.com/wzpan/QtEVM>                            */
/*      Nick D'Ademo    <https://github.com/nickdademo/qt-opencv-multithreaded>     */
/*                                                                                  */
/* Realtime-Video-Magnification->PlayerThread.cpp                                   */
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

#include "main/threads/PlayerThread.h"

// Constructor
PlayerThread::PlayerThread(const std::string filepath, int width, int height, double fps)
    : QThread(),
      filepath(filepath),
      width(width),
      height(height),
      fps(fps),
      emitOriginal(false)
{
    doStop = true;
    doPause = false;
    doPlay = false;
    statsData.averageFPS = 0;
    statsData.averageVidProcessingFPS = 0;
    statsData.nFramesProcessed = 0;

    originalBuffer = std::vector<cv::Mat>();
    processingBuffer = std::vector<cv::Mat>();

    imgProcSettings.framerate = fps;

    sampleNumber = 0;
    fpsSum = 0;
    fpsQueue.clear();

    frameNum = 0;
    prevFrameNum = 0;
    breathValues[3];
    prevSumm = 0;
    this->magnificator = Magnificator(&processingBuffer, &imgProcFlags, &imgProcSettings, &frameNum);
    this->cap = cv::VideoCapture();
    currentWriteIndex = 0;
}

// Destructor
PlayerThread::~PlayerThread()
{
    doStopMutex.lock();
    doStop = true;
    if(releaseFile())
        qDebug() << "Released File.";
    doStopMutex.unlock();
    wait();
}

// Thread
void PlayerThread::run()
{
    // The standard delay time to keep FPS playing rate without processing time
    double delay = 1000.0/fps;
    qDebug() << "Starting player thread...";
    QElapsedTimer mTime;

    // Shared memory init
    HANDLE hMapFile;
    LPCTSTR pBuf;

    int temp;
    int *point2;
    point2 = &temp;


    TCHAR szName[]=TEXT("ReimaginingBreath");
    TCHAR szMsg[]=TEXT("YOssage from first process.");


    hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,    // use paging file
        NULL,                    // default security
        PAGE_READWRITE,          // read/write access
        0,                       // maximum object size (high-order DWORD)
        BUF_SIZE,                // maximum object size (low-order DWORD)
        szName);                 // name of mapping object

    if (hMapFile == NULL)
    {
        _tprintf(TEXT("Could not create file mapping object (%d).\n"),
                 GetLastError());
    }
    pBuf = (LPTSTR) MapViewOfFile(hMapFile,   // handle to map object
                                  FILE_MAP_ALL_ACCESS, // read/write permission
                                  0,
                                  0,
                                  BUF_SIZE);

    if (pBuf == NULL)
    {
        _tprintf(TEXT("Could not map view of file (%d).\n"),
                 GetLastError());

        CloseHandle(hMapFile);
    }

    // end shared memory init

    /////////////////////////////////////
    /// Stop thread if doStop=TRUE /////
    ///////////////////////////////////
    int prevFrameNum = 0;
    float prevSumm;
    while(!doStop)
    {
        //////////////////////////////////////////////
        /// Stop thread if processed all frames /////
        ////////////////////////////////////////////
        if(currentWriteIndex >= lengthInFrames) {
            endOfFrame_action();
            break;
        }

        // Save process time
        processingTime = t.elapsed();
        // Start timer
        t.start();

        /////////////////////////////////////
        /// Pause thread if doPause=TRUE ///
        ///////////////////////////////////
        doStopMutex.lock();
        if(doPause)
        {
            doPlay = false;
            prevFrameNum = frameNum; // TODO maybe just thjis not the if >2?
            doStopMutex.unlock();
            break;
        }
        doStopMutex.unlock();

        // Start timer and capture time needed to process 1 frame here
        if(getCurrentReadIndex() == processingBufferLength-1)
            mTime.start();
        // Switch to process images on the fly instead of processing a whole buffer, reducing MEM
        if(imgProcFlags.colorMagnifyOn && processingBufferLength > 2 && magnificator.getBufferSize() > 2) {
            processingBufferLength = 2;
        }

        ///////////////////////////////////
        /////////// Capturing ////////////
        /////////////////////////////////
        // Fill buffer, check if it's the start of magnification or not
        for(int i = processingBuffer.size(); i < processingBufferLength && getCurrentFramenumber() < lengthInFrames; i++) {
            processingMutex.lock();

            // Try to grab the next Frame
            if(cap.read(grabbedFrame)) {
                // Preprocessing
                // Set ROI of frame
                currentFrame = cv::Mat(grabbedFrame.clone(), currentROI);
                // Convert to grayscale
                if(imgProcFlags.grayscaleOn && (currentFrame.channels() == 3 || currentFrame.channels() == 4)) {
                    cvtColor(currentFrame, currentFrame, cv::COLOR_BGR2GRAY, 1);
                }

                // Fill fuffer
                processingBuffer.push_back(currentFrame);
                if(emitOriginal)
                    originalBuffer.push_back(currentFrame.clone());
            }

            // Wasn't able to grab frame, abort thread
            else {
                processingMutex.unlock();
                endOfFrame_action();
                break;
            }

            processingMutex.unlock();
        }
        // Breakpoint if grabbing frames wasn't succesful
        if(doStop) {
            break;
        }

        ///////////////////////////////////
        /////////// Magnifying ///////////
        /////////////////////////////////
        processingMutex.lock();

        if(imgProcFlags.colorMagnifyOn)
        {
            magnificator.colorMagnify();
            if(magnificator.hasFrame())
            {
                currentFrame = magnificator.getFrameFirst();
            }
        }
        else if(imgProcFlags.laplaceMagnifyOn)
        {
            magnificator.laplaceMagnify();
            frameNum++;
            if(magnificator.hasFrame())
            {
                currentFrame = magnificator.getFrameFirst();
            }
        }
        else if(imgProcFlags.rieszMagnifyOn)
        {
            magnificator.rieszMagnify();
            if(magnificator.hasFrame())
            {
                currentFrame = magnificator.getFrameFirst();
            }
        }
        else {
            frameNum = 0;
            prevFrameNum = 0;
            // Read frames unmagnified
            currentFrame = processingBuffer.at(getCurrentReadIndex());
            // Erase to keep buffer size
            processingBuffer.erase(processingBuffer.begin());
        }
        // Increase number of frames given to GUI
        currentWriteIndex++;

        temp = magnificator.breathMeasureOutput;

        // keep array index in bounds
        if ((frameNum -1 - prevFrameNum) > 2 || (frameNum -1 - prevFrameNum) < 0) {
            prevFrameNum = frameNum;
        }
        breathValues[frameNum-1 - prevFrameNum] = temp;
        if (frameNum - prevFrameNum == 3) {


            float summ = 0;
            for (int i = 0; i < 3; i++) {
                summ += breathValues[i];
            }
            summ /= 3;

            // for first one, initialize prevSumm.
            if (frameNum == 3) {
                prevSumm = summ;
            }

            float slope = (breathValues[2] - breathValues[0])/3;

            // if massive jump, make slope +/-25.
            if (prevSumm != 0) {
                if ((summ - prevSumm)/2 > 25) {
                    summ = prevSumm + 50;
                }
                else if ((summ - prevSumm)/2 < -25) {
                    summ = prevSumm - 50;
                }
            }

            temp = summ;


            CopyMemory((PVOID)pBuf, point2, sizeof(int));

            QFile file("out.csv");
            if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
                if(!file.isOpen())
                {
                    //alert that file did not open
                    cout << "Couldn't open file";
                }

                QTextStream outStream(&file);
                outStream << frameNum << "," << summ << "\n";

                file.close();
            }

            prevFrameNum = frameNum;
            prevSumm = summ;
        }


//        cv::putText(currentFrame, //target image
//                    "FRAME " + std::to_string(frameNum) + ", " + std::to_string(prevFrameNum), //text
//                    cv::Point(10, currentFrame.rows / 4), //top-left position
//                    cv::FONT_HERSHEY_DUPLEX,
//                    1.0,
//                    CV_RGB(118, 185, 0), //font color
//                    2);
        frame = MatToQImage(currentFrame);
        if(emitOriginal) {
            originalFrame = MatToQImage(originalBuffer.front());
            if(!originalBuffer.empty()) {
                originalBuffer.erase(originalBuffer.begin());
                frameNum = 0;
            }
        }

        processingMutex.unlock();

        ///////////////////////////////////
        /////////// Updating /////////////
        /////////////////////////////////
        // Inform GUI thread of new frame
        emit newFrame(frame);
        // Inform GUI thread of original frame if option was set
        if(emitOriginal)
            emit origFrame(originalFrame);

        // Update statistics
        updateFPS(processingTime);
        statsData.nFramesProcessed = currentWriteIndex;
        // Inform GUI about updatet statistics
        emit updateStatisticsInGUI(statsData);

        // To keep FPS playing rate, adjust waiting time, dependent on the time that is used to process one frame
        double diff = mTime.elapsed();
        int wait = max(delay-diff,0.0);
        this->msleep(wait);
    }
    UnmapViewOfFile(pBuf);

    CloseHandle(hMapFile);

    qDebug() << "Stopping player thread...";
}

int PlayerThread::getCurrentReadIndex()
{
    return std::min(currentWriteIndex, processingBufferLength-1);
}

double PlayerThread::getFPS()
{
    return fps;
}

void PlayerThread::getOriginalFrame(bool doEmit)
{
    QMutexLocker locker1(&doStopMutex);
    QMutexLocker locker2(&processingMutex);

    originalBuffer.clear();
    originalBuffer = processingBuffer;
    emitOriginal = doEmit;
}

// Load videofile
bool PlayerThread::loadFile()
{
    // Just in case, release file
    releaseFile();
    cap = cv::VideoCapture(filepath);

    // Open file
    bool openResult = isFileLoaded();
    if(!openResult)
        openResult = cap.open(filepath);

    // Set resolution
    if(width != -1)
        cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
    if(height != -1)
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
    if(fps == -1) {
        fps = cap.get(cv::CAP_PROP_FPS);
    }

    // OpenCV can't read all mp4s properly, fps is often false
    if(fps < 0 || !std::isfinite(fps))
    {
        fps = 30;
    }

    // initialize Buffer length
    setBufferSize();

    // Write information in Settings
    statsData.averageFPS = fps;
    imgProcSettings.framerate = fps;
    imgProcSettings.frameHeight = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    imgProcSettings.frameWidth = cap.get(cv::CAP_PROP_FRAME_WIDTH);

    // Save total length of video
    lengthInFrames = cap.get(cv::CAP_PROP_FRAME_COUNT);

    return openResult;
}

// Release the file from VideoCapture
bool PlayerThread::releaseFile()
{
    // File is loaded
    if(cap.isOpened())
    {
        // Release File
        cap.release();
        return true;
    }
    // File is NOT laoded
    else
        return false;
}

// Stop the thread
void PlayerThread::stop()
{
    doStop = true;
    doPause = false;
    doPlay = false;

    setBufferSize();
    releaseFile();

    currentWriteIndex = 0;
}

void PlayerThread::endOfFrame_action()
{
    doStop = true;
    stop();
    emit endOfFrame();
    statsData.nFramesProcessed = 0;
    emit updateStatisticsInGUI(statsData);
}

bool PlayerThread::isFileLoaded() {
    return cap.isOpened();
}

int PlayerThread::getInputSourceWidth()
{
    return imgProcSettings.frameWidth;
}

int PlayerThread::getInputSourceHeight()
{
    return imgProcSettings.frameHeight;
}

void PlayerThread::setROI(QRect roi)
{
    QMutexLocker locker1(&doStopMutex);
    QMutexLocker locker2(&processingMutex);
    currentROI.x = roi.x();
    currentROI.y = roi.y();
    currentROI.width = roi.width();
    currentROI.height = roi.height();
    int levels = magnificator.calculateMaxLevels(roi);
    magnificator.clearBuffer();
    locker1.unlock();
    locker2.unlock();
    setBufferSize();
    emit maxLevels(levels);
}

QRect PlayerThread::getCurrentROI()
{
    return QRect(currentROI.x, currentROI.y, currentROI.width, currentROI.height);
}

// Private Slots
void PlayerThread::updateImageProcessingFlags(struct ImageProcessingFlags imgProcessingFlags)
{
    QMutexLocker locker1(&doStopMutex);
    QMutexLocker locker2(&processingMutex);
    this->imgProcFlags.grayscaleOn = imgProcessingFlags.grayscaleOn;
    this->imgProcFlags.colorMagnifyOn = imgProcessingFlags.colorMagnifyOn;
    this->imgProcFlags.laplaceMagnifyOn = imgProcessingFlags.laplaceMagnifyOn;
    this->imgProcFlags.rieszMagnifyOn = imgProcessingFlags.rieszMagnifyOn;
    locker1.unlock();
    locker2.unlock();

    setBufferSize();
}

void PlayerThread::updateImageProcessingSettings(struct ImageProcessingSettings imgProcessingSettings)
{
    QMutexLocker locker1(&doStopMutex);
    QMutexLocker locker2(&processingMutex);
    bool resetBuffer = (this->imgProcSettings.levels != imgProcessingSettings.levels);

    this->imgProcSettings.amplification = imgProcessingSettings.amplification;
    this->imgProcSettings.coWavelength = imgProcessingSettings.coWavelength;
    this->imgProcSettings.coLow = imgProcessingSettings.coLow;
    this->imgProcSettings.coHigh = imgProcessingSettings.coHigh;
    this->imgProcSettings.chromAttenuation = imgProcessingSettings.chromAttenuation;
    this->imgProcSettings.levels = imgProcessingSettings.levels;

    if(resetBuffer) {
        locker1.unlock();
        locker2.unlock();
        setBufferSize();
    }
}

// Public Slots / Video control
void PlayerThread::playAction()
{
    if(!isPlaying()) {

        if(!cap.isOpened())
            loadFile();

        if(isPausing()) {
            doStop = false;
            doPause = false;
            doPlay = true;
            cap.set(cv::CAP_PROP_POS_FRAMES, getCurrentFramenumber());
            start();
        }
        else if(isStopping()) {
            doStop = false;
            doPause = false;
            doPlay = true;
            start();
        }
    }
}
void PlayerThread::stopAction()
{
    stop();
}
void PlayerThread::pauseAction()
{
    doStop = false;
    doPause = true;
    doPlay = false;
}

void PlayerThread::pauseThread()
{
    pauseAction();
}

bool PlayerThread::isPlaying()
{
    return this->doPlay;
}

bool PlayerThread::isStopping()
{
    return this->doStop;
}

bool PlayerThread::isPausing()
{
    return this->doPause;
}

void PlayerThread::setCurrentFrame(int framenumber)
{
    currentWriteIndex = framenumber;
    setBufferSize();
}

void PlayerThread::setCurrentTime(int ms)
{
    if(cap.isOpened())
        cap.set(cv::CAP_PROP_POS_MSEC, ms);
}

double PlayerThread::getInputFrameLength()
{
    return lengthInFrames;
}

double PlayerThread::getInputTimeLength() {
    return lengthInMs;
}

double PlayerThread::getCurrentFramenumber() {
    return cap.get(cv::CAP_PROP_POS_FRAMES);
}

double PlayerThread::getCurrentPosition() {
    return cap.get(cv::CAP_PROP_POS_MSEC);
}

void PlayerThread::updateFPS(int timeElapsed)
{
    // Add instantaneous FPS value to queue
    if(timeElapsed>0)
    {
        fpsQueue.enqueue((int)1000/timeElapsed);
        // Increment sample number
        sampleNumber++;
    }

    // Maximum size of queue is DEFAULT_PROCESSING_FPS_STAT_QUEUE_LENGTH
    if(fpsQueue.size()>PROCESSING_FPS_STAT_QUEUE_LENGTH)
        fpsQueue.dequeue();

    // Update FPS value every DEFAULT_PROCESSING_FPS_STAT_QUEUE_LENGTH samples
    if((fpsQueue.size()==PROCESSING_FPS_STAT_QUEUE_LENGTH)&&(sampleNumber==PROCESSING_FPS_STAT_QUEUE_LENGTH))
    {
        // Empty queue and store sum
        while(!fpsQueue.empty())
            fpsSum+=fpsQueue.dequeue();
        // Calculate average FPS
        statsData.averageVidProcessingFPS=fpsSum/PROCESSING_FPS_STAT_QUEUE_LENGTH;
        // Reset sum
        fpsSum=0;
        // Reset sample number
        sampleNumber=0;
    }
}

// Magnificator
void PlayerThread::fillProcessingBuffer()
{
    processingBuffer.push_back(currentFrame);
}

bool PlayerThread::processingBufferFilled()
{
    return (processingBuffer.size() == processingBufferLength);
}

void PlayerThread::setBufferSize()
{
    QMutexLocker locker1(&doStopMutex);
    QMutexLocker locker2(&processingMutex);

    processingBuffer.clear();
    originalBuffer.clear();
    magnificator.clearBuffer();

    if(imgProcFlags.colorMagnifyOn) {
        processingBufferLength = magnificator.getOptimalBufferSize(imgProcSettings.framerate);
    }
    else if(imgProcFlags.laplaceMagnifyOn) {
        processingBufferLength = 2;
    }
    else if(imgProcFlags.rieszMagnifyOn) {
        processingBufferLength = 2;
    }
    else {
        processingBufferLength = 1;
    }

    if(cap.isOpened() || !doStop)
        cap.set(cv::CAP_PROP_POS_FRAMES, std::max(currentWriteIndex-processingBufferLength,0));
}
