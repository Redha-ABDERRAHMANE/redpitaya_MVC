#include <iostream>
#include <opencv2/imgproc.hpp>
#include <VmbCPP/VmbCPP.h>
#include <QObject>
#include <QImage>
using namespace VmbCPP;
using namespace cv;






class FrameObserver : public IFrameObserver {
private:
    CameraPtr m_pCamera;
    Mat m_frame;
    bool m_frameReady;
    std::mutex m_frameMutex;

public:
    FrameObserver(CameraPtr pCamera) : IFrameObserver(pCamera), m_pCamera(pCamera), m_frameReady(false) {}

    void FrameReceived(const FramePtr pFrame) override {
        if (!pFrame) return;

        VmbUchar_t* pBuffer;
        VmbUint32_t bufferSize;

        if (pFrame->GetBuffer(pBuffer) == VmbErrorSuccess &&
            pFrame->GetBufferSize(bufferSize) == VmbErrorSuccess) {

            VmbUint32_t width, height;
            if (pFrame->GetWidth(width) != VmbErrorSuccess || pFrame->GetHeight(height) != VmbErrorSuccess) {
                m_pCamera->QueueFrame(pFrame);
                return;
            }

            VmbPixelFormatType pixelFormat;
            pFrame->GetPixelFormat(pixelFormat);

            std::lock_guard<std::mutex> lock(m_frameMutex);

            try {
                // Convert to OpenCV Mat based on pixel format
                if (pixelFormat == VmbPixelFormatMono8) { // CLONE IS IMPORT, VIMBA API DELETES POINTER AT THE END OF CALLBACK, ASSIGNING m_frame the value directly could lead to memory issues.
                    m_frame = Mat(height, width, CV_8UC1, pBuffer).clone();
                    m_frameReady = true;
                }
                else if (pixelFormat == VmbPixelFormatBgr8) {
                    m_frame = Mat(height, width, CV_8UC3, pBuffer).clone();
                    m_frameReady = true;
                }
                else if (pixelFormat == VmbPixelFormatRgb8) {
                    Mat temp = Mat(height, width, CV_8UC3, pBuffer);
                    cvtColor(temp, m_frame, COLOR_RGB2BGR);
                    m_frameReady = true;
                }
                else {
                    // For other formats, try to convert to mono8
                    m_frame = Mat(height, width, CV_8UC1, pBuffer).clone();
                    m_frameReady = true;
                }
            }
            catch (const cv::Exception& e) {
                std::cerr << "OpenCV error in frame processing: " << e.what() << std::endl;
                m_frameReady = false;
            }
        }

        // Re-queue the frame
        m_pCamera->QueueFrame(pFrame);
    }

    bool GetFrame(Mat& frame) {
        std::lock_guard<std::mutex> lock(m_frameMutex);
        if (m_frameReady && !m_frame.empty()) {
            frame = m_frame.clone();
            m_frameReady = false;
            return true;
        }
        return false;
    }
};
///////////////////////////////////////////////////////////////////////////////////////////////

class CameraInputWorker : public QObject {
    Q_OBJECT

private:
    VmbSystem& sys;
    CameraPtrVector cameras;
    CameraPtr camera;
    FeaturePtr pixelFormatFeature;
    FeaturePtr widthFeature, heightFeature;
    FeaturePtr payloadSizeFeature;
    FeaturePtr acquisitionStartFeature;
    FeaturePtr saturationFeature;
    FeaturePtr exposureFeature;
    FeaturePtr framerateControlEnableFeature;
    FeaturePtr framerateControlFeature;
    VmbInt64_t width = 640, height = 480;
    std::shared_ptr<FrameObserver> frameObserver = nullptr;
    IFrameObserverPtr observerPtr;
    VmbInt64_t payloadSize = 0;

    FramePtrVector frames;


    bool running = true;
    VmbErrorType err;

    double exposureTime = 0.0;
    double saturationValue=1.0;





private:
    QImage MatToQImage(const cv::Mat& mat) {
        if (mat.type() == CV_8UC1) {
            return QImage(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_Grayscale8).copy();
        }
        else if (mat.type() == CV_8UC3) {
            cv::Mat rgb;
            cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
            return QImage(rgb.data, rgb.cols, rgb.rows, static_cast<int>(rgb.step), QImage::Format_RGB888).copy();
        }
        else {
            return QImage(); // Unsupported format
        }
    }
    bool cameraStartup() {
        err = sys.Startup();
        if (err != VmbErrorSuccess) {
            std::cout << "Failed to start Vimba: " << err << std::endl;
            return false;
        }
        std::cout << "Vimba started successfully" << std::endl;
        return true;
    }

    bool getCamera() {
        err = sys.GetCameras(cameras);

        if (cameras.empty()) {
            std::cout << "No cameras found" << std::endl;
            sys.Shutdown();
            return false;
        }
        std::cout << "Found " << cameras.size() << " camera(s)" << std::endl;
        camera = cameras[0];
        return true;

    }

    bool getCameraInfo() {

        // Get camera info
        std::string cameraID;
        camera->GetID(cameraID);
        std::cout << "Using camera: " << cameraID << std::endl;

        err = camera->Open(VmbAccessModeFull);
        if (err != VmbErrorSuccess) {
            std::cout << "Failed to open camera: " << err << std::endl;
            sys.Shutdown();
            return false;
        }
        std::cout << "Camera opened successfully" << std::endl;
        return true;
    }

    void setPixelFormat() {

        // Set pixel format (try Mono8 first, then BGR8)

        err = camera->GetFeatureByName("PixelFormat", pixelFormatFeature);
        if (err == VmbErrorSuccess) {

            err = pixelFormatFeature->SetValue("Mono8");
            if (err != VmbErrorSuccess) {

                std::cout << "Mono8 not supported, trying BGR8..." << std::endl;
                err = pixelFormatFeature->SetValue("BGR8");

                if (err != VmbErrorSuccess) {
                    std::cout << "BGR8 not supported either, using default format" << std::endl;
                }
            }
        }
    }


    // Get frame dimensions

    void setFrameDimensions() {

        if (camera->GetFeatureByName("Width", widthFeature) == VmbErrorSuccess) {
            widthFeature->GetValue(width);
        }
        if (camera->GetFeatureByName("Height", heightFeature) == VmbErrorSuccess) {
            heightFeature->GetValue(height);
        }

        std::cout << "Frame size: " << width << "x" << height << std::endl;
    }
    // Create frame observer - use shared_ptr for proper memory management
    void setFrameObserver() {
        frameObserver = std::make_shared<FrameObserver>(camera);
        observerPtr = IFrameObserverPtr(frameObserver.get());
    }

    // Calculate proper buffer size based on pixel format
    void calculateBufferSize() {
        if (camera->GetFeatureByName("PayloadSize", payloadSizeFeature) == VmbErrorSuccess) {
            payloadSizeFeature->GetValue(payloadSize);
            std::cout << "Payload size: " << payloadSize << " bytes" << std::endl;
        }
        else {
            payloadSize = width * height * 3; // Default assumption
        }
    }

    bool prepareFrame() {
        // Prepare frames for acquisition
         // Use 3 frames for buffering
        for (auto& frame : frames) {
            frame.reset(new Frame(payloadSize));
            frame->RegisterObserver(observerPtr);
        }

        // Announce frames to camera
        for (auto& frame : frames) {
            camera->AnnounceFrame(frame);
        }

        // Start capture engine
        err = camera->StartCapture();
        if (err != VmbErrorSuccess) {
            std::cout << "Failed to start capture: " << err << std::endl;
            camera->Close();
            sys.Shutdown();
            return false;
        }
        // Queue frames
        for (auto& frame : frames) {
            camera->QueueFrame(frame);
        }
        return true;
    }

    bool startAcquisition() {
        // Start acquisition

        err = camera->GetFeatureByName("AcquisitionStart", acquisitionStartFeature);
        if (err == VmbErrorSuccess) {
            err = acquisitionStartFeature->RunCommand();
            if (err != VmbErrorSuccess) {
                std::cout << "Failed to start acquisition: " << err << std::endl;
                camera->EndCapture();
                camera->Close();
                sys.Shutdown();
                return false;
            }
        }
        return true;
    }

    bool stopAcquisitionAndSysShutdown() {
        err= camera->EndCapture();
        if (err == VmbErrorSuccess) {
            camera->Close();
            sys.Shutdown();
            return true;
        }
        std::cout << "Failed to stop acquisition" << std::endl;
        return false;

    }



public :
    CameraInputWorker() : sys(VmbSystem::GetInstance()),frames(3){}

    void startCamera() {
            cameraStartup();
            getCamera();
            getCameraInfo();
            setPixelFormat();
            setFrameDimensions();
            setFrameObserver();
            calculateBufferSize();
            prepareFrame();
            startAcquisition();

    }


signals:

    void ImageReceived(const QImage& image);

public slots:
    


   

    void setSaturationValue(const double& value) {
        saturationFeature->SetValue(value);

    }

    void setExposureTimeValue(const double& value) {
        exposureFeature->SetValue(value);

    }


        // Display loop
    void getDisplayFrame() {

        Mat displayFrame;
        int frameCount = 0;
        std::cout << "entered camera get frame " << std::endl;

        while (running) {
            if (frameObserver->GetFrame(displayFrame)) {
                frameCount++;
                if (frameCount % 30 == 0) {
                    std::cout << "Received " << frameCount << " frames" << std::endl;
                }

                if (!displayFrame.empty()) {
                    try {

                        emit ImageReceived(MatToQImage(displayFrame));
                        
                    }
                    catch (const cv::Exception& e) {
                        std::cerr << "Error displaying frame: " << e.what() << std::endl;
                    }
                }
            }

        }

        std::cout << "Stopping acquisition..." << std::endl;
    }

    bool endOfWork() {

        // Proper cleanup sequence
        running = false;
        FeaturePtr acquisitionStopFeature;
        err = camera->GetFeatureByName("AcquisitionStop", acquisitionStopFeature);
        if (err == VmbErrorSuccess) {
            acquisitionStopFeature->RunCommand();
        }

        camera->EndCapture();

        // Flush the frame queue
        camera->FlushQueue();

        // Revoke frames
        for (auto& frame : frames) {
            camera->RevokeFrame(frame);
        }

        camera->Close();
        sys.Shutdown();

        return true;
    }
};