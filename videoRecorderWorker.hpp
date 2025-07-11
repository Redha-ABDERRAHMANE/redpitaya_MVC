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

class VideoRecorderThread : public QObject {
    Q_OBJECT
private:
    QProcess* ffmpegProcess = nullptr;

public:


public slots:
    void ReceiveImageToCapture(const QImage& image) {
        static int i = 0;
        std::cout << "Image received to capture\n";

        // Convert to RGB24 format (what FFmpeg expects)
        //static QImage rgbImage = image.convertToFormat(QImage::Format_RGB888);

        // Write raw RGB data directly
        const uchar* data = image.constBits();
        int dataSize = image.sizeInBytes();

        ffmpegProcess->write(reinterpret_cast<const char*>(data), dataSize);
    }

    void startRecording() {
        std::cout << "start record\n";
        if (ffmpegProcess == nullptr) {
            ffmpegProcess = new QProcess(this);
        }
        QStringList args;

        // Get image dimensions (you'll need to pass these or make them class members)
        int width = 2592;  // Replace with actual width
        int height = 1944; // Replace with actual height
        

        args << "-f" << "rawvideo"
            << "-pix_fmt" << "rgb24"
            << "-s" << QString("%1x%2").arg(width).arg(height)
            << "-r" << "25"
            << "-i" << "-"
            << "-vcodec" << "libx264"
            << "-pix_fmt" << "yuv420p"  // Common output format
            << "-y"
            << "C:/Users/Redha/Pictures/testcapture/output.mp4";

        ffmpegProcess->start("ffmpeg", args);
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