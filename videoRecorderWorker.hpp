#ifndef VIDEORECORDERWORKER_H
#define VIDEORECORDERWORKER_H

#include <QObject>
#include <QThread>
#include <QProcess>
#include <QStringList>

#include <QImage>
#include <QByteArray>
#include <QBuffer>
#include <QIODevice>
#include <iostream>
#include <QCoreApplication>
#include <commonValues.h>

class VideoRecorderThread : public QObject {
    Q_OBJECT
private:
    QProcess* ffmpegProcess = nullptr;

public:


public slots:
    void ReceiveImageToCapture(const QImage& image) {
        static int i = 0;
        std::cout << "Image received to capture\n";
        if (ffmpegProcess==nullptr) return;

 

        // Write raw RGB data directly
        const uchar* data = image.constBits();
        int dataSize = image.sizeInBytes();

        ffmpegProcess->write(reinterpret_cast<const char*>(data), dataSize);
    }

    void startRecording() {
        static int i = 0;
        std::cout << "start record\n";
        if (ffmpegProcess == nullptr) {
            ffmpegProcess = new QProcess(this);
            connect(ffmpegProcess, &QProcess::errorOccurred, [this](QProcess::ProcessError error) {
                std::cerr << "FFmpeg process error: " << error << std::endl;
                });

            connect(ffmpegProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                [this](int exitCode, QProcess::ExitStatus exitStatus) {
                    std::cout << "FFmpeg finished with exit code: " << exitCode << std::endl;
                });
        }
        QStringList args;

        
        int width = FRAMEWIDTH; 
        int height = FRAMEHEIGHT;
        

        args << "-f" << "rawvideo"
            << "-pix_fmt" << "gray"
            << "-s" << QString("%1x%2").arg(width).arg(height)
            << "-r" << "25"
            << "-i" << "-"
            << "-vcodec" << "libx264"
            << "-preset" << "ultrafast"  // Move here
            << "-pix_fmt" << "yuv420p"
            << "-y"
            << QString("C:/Users/Redha/Pictures/testcapture/output_%1.mp4").arg(i);

        ffmpegProcess->start("ffmpeg", args);
        i++;
        if (!ffmpegProcess->waitForStarted(5000)) {
            std::cerr << "Failed to start FFmpeg process" << std::endl;
            return;
        }
        std::cout << "FFmpeg process started successfully" << std::endl;
    }
    void stopRecording() {

        // Close stdin to signal end of input
        if (ffmpegProcess->state() == QProcess::Running) {
            ffmpegProcess->closeWriteChannel();

            // Wait for FFmpeg to finish with timeout
            if (!ffmpegProcess->waitForFinished(15000)) { // 15 second timeout
                std::cout << "FFmpeg didn't finish in time, terminating..." << std::endl;
                ffmpegProcess->terminate();

                if (!ffmpegProcess->waitForFinished(3000)) {
                    std::cout << "Force killing FFmpeg process..." << std::endl;
                    ffmpegProcess->kill();
                    ffmpegProcess->waitForFinished(1000);
                }
            }
        }

        // Clean up process object
        ffmpegProcess->deleteLater();
        ffmpegProcess = nullptr;

        std::cout << "Recording stopped and cleaned up" << std::endl;

        // Force event processing to ensure cleanup
        QCoreApplication::processEvents();
    }
};



































#endif // VIDEORECORDERWORKER_H