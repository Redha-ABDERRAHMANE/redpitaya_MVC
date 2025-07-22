#include <iostream>
#include <VmbCPP/VmbCPP.h>
#include <QObject>
#include <QImage>
#include<QCoreApplication>
#include <QThread>
#include <chrono>
using namespace VmbCPP;



#define ANGLE_180 180
#define ANGLE_NULL 0






class FrameObserver : public IFrameObserver {
private:
    CameraPtr m_pCamera;
    QImage m_frame;
    bool m_frameReady;
    std::atomic<bool> m_shutdown;  // Add shutdown flag
    std::mutex m_frameMutex;

public:
    FrameObserver(CameraPtr pCamera)
        : IFrameObserver(pCamera), m_pCamera(pCamera), m_frameReady(false), m_shutdown(false) {
    }

    // Add shutdown method to be called before destruction
    void Shutdown() {
        std::lock_guard<std::mutex> lock(m_frameMutex);
        m_shutdown = true;
        m_frameReady = false;
 
        m_frame = QImage();
        //m_frame.release();  // Release OpenCV Mat resources
    }

    void FrameReceived(const FramePtr pFrame) override {
        // Early exit if shutting down
        {
            std::lock_guard<std::mutex> lock(m_frameMutex);
            if (m_shutdown) {
                // Still need to re-queue the frame even during shutdown
                if (m_pCamera && pFrame) {
                    m_pCamera->QueueFrame(pFrame);
                }
                return;
            }
        }

        if (!pFrame || !m_pCamera) {
            return;
        }

        VmbUchar_t* pBuffer;
        VmbUint32_t bufferSize;
        if (pFrame->GetBuffer(pBuffer) == VmbErrorSuccess &&
            pFrame->GetBufferSize(bufferSize) == VmbErrorSuccess) {

            VmbUint32_t width, height;
            if (pFrame->GetWidth(width) != VmbErrorSuccess ||
                pFrame->GetHeight(height) != VmbErrorSuccess) {
                m_pCamera->QueueFrame(pFrame);
                return;
            }

            VmbPixelFormatType pixelFormat;
            pFrame->GetPixelFormat(pixelFormat);

            std::lock_guard<std::mutex> lock(m_frameMutex);

            // Double-check shutdown status after acquiring lock
            if (m_shutdown) {
                m_pCamera->QueueFrame(pFrame);
                return;
            }

            try {
                // Convert to OpenCV Mat based on pixel format
                if (pixelFormat == VmbPixelFormatMono8) {
                    m_frame = QImage((uchar*)pBuffer, (int)width, (int)height, QImage::Format_Grayscale8);
                    m_frameReady = true;
                       
                }
                else if (pixelFormat == VmbPixelFormatBgr8) {
                    m_frame = QImage((uchar*)pBuffer, (int)width, (int)height, QImage::Format_BGR888);
                    m_frameReady = true;
                }
                else if (pixelFormat == VmbPixelFormatRgb8) {
                    m_frame = QImage((uchar*)pBuffer, (int)width, (int)height, QImage::Format_RGB888);
                    
                    m_frameReady = true;
                }
                else {
                    // For other formats, try to convert to mono8
                    m_frame = QImage((uchar*)pBuffer, (int)width, (int)height, QImage::Format_Grayscale8);
                    m_frameReady = true;
                }
            }
            catch (std::exception e) {
                std::cerr << " error in frame processing : " << e.what()<< std::endl;
                m_frameReady = false;
            }

        }

        // Re-queue the frame - check if camera is still valid
        if (m_pCamera && pFrame) {
            m_pCamera->QueueFrame(pFrame);
        }
    }

    bool GetFrame(QImage& frame) {
        std::lock_guard<std::mutex> lock(m_frameMutex);
        if (m_shutdown) {
            return false;  // Don't provide frames during shutdown
        }

        if (m_frameReady && !m_frame.isNull()) {
            frame = m_frame;
            m_frameReady = false;
            return true;
        }
        return false;
    }

    // Destructor to ensure cleanup
    ~FrameObserver() {
        Shutdown();
        std::cout << "shutdown called \n";
    }
};
///////////////////////////////////////////////////////////////////////////////////////////////

class CameraInputWorker :public QObject {
    Q_OBJECT

private:
    VmbSystem& sys;
    CameraPtrVector cameras;
    CameraPtr camera;
    FeaturePtr pixelFormatFeature;
    FeaturePtr widthFeature;
    FeaturePtr heightFeature;
    FeaturePtr payloadSizeFeature;
    FeaturePtr acquisitionStartFeature;
    FeaturePtr acquisitionStopFeature;
    FeaturePtr saturationFeature;
    FeaturePtr exposureFeature;
    FeaturePtr framerateControlEnableFeature;
    FeaturePtr framerateControlFeature;
    FeaturePtr imageReverseXFeature;
    FeaturePtr imageReverseYFeature;
    FeaturePtr balanceWhiteAutoFeature;
    VmbInt64_t width = 2592, height = 1944;
    FrameObserver* frameObserver = nullptr;
    IFrameObserverPtr observerPtr;
    VmbInt64_t payloadSize = 0;

    FramePtrVector frames;
    FeaturePtr triggerMode;


    bool running = true;

    bool recordingVideo = false;
    std::mutex m_recordingMutex;
    VmbErrorType err;

    double exposureTime = 0.0;
    double saturationValue=1.0;
    double framerate = 30;




private:
    QImage MatToQImage(const QImage& mat) {
        //if (mat.type() == CV_8UC1) {
        //    return QImage(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_Grayscale8).copy();
        //}
        //else if (mat.type() == CV_8UC3) {
        //    cv::Mat rgb;
        //    cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
        //    return QImage(rgb.data, rgb.cols, rgb.rows, static_cast<int>(rgb.step), QImage::Format_RGB888).copy();
        //}
        //else {
        //    return QImage(); // Unsupported format
        //}
        return mat.copy();
    }
    bool CameraStartup() {
        err = sys.Startup();
        if (err != VmbErrorSuccess) {
            std::cout << "Failed to start Vimba: " << err << std::endl;
            return false;
        }
        std::cout << "Vimba started successfully" << std::endl;
        return true;
    }

    bool GetCamera() {
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

    bool GetCameraInfo() {

        // Get camera info
        std::string cameraID;
        if (camera == nullptr) { return false; }
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

    bool GetSaturationAndExposureTimeFeaturePtr() {
        if (camera->GetFeatureByName("ExposureTime", exposureFeature) != VmbErrorSuccess) return false;
        if (camera->GetFeatureByName("Saturation", saturationFeature) != VmbErrorSuccess) return false;
        return true;
    }

    bool SetPixelFormat() {

        // Set pixel format (try Mono8 first, then BGR8)

        err = camera->GetFeatureByName("PixelFormat", pixelFormatFeature);
        if (err == VmbErrorSuccess) {

            err = pixelFormatFeature->SetValue("Mono8"); //RGB8
            if (err != VmbErrorSuccess) {

                std::cout << "Mono8 not supported, trying BGR8..." << std::endl;
                err = pixelFormatFeature->SetValue("BGR8");

                if (err != VmbErrorSuccess) {
                    std::cout << "BGR8 not supported either, using default format" << std::endl;
                }
            }
            return true;
        }
        std::cout << "could not get pixel format feature\n";
        return false;
    }


    // Get frame dimensions

    bool SetFrameDimensions() {

        if (camera->GetFeatureByName("Width", widthFeature) != VmbErrorSuccess) {
            return false;
            
        }
        if (camera->GetFeatureByName("Height", heightFeature) != VmbErrorSuccess) {
            return false;
        }
        widthFeature->SetValue(width);
        heightFeature->SetValue(height);
        widthFeature->GetValue(width);
        heightFeature->GetValue(height);

        std::cout << "Frame size: " << width << "x" << height << std::endl;
        return true;

    }


    bool SetWhiteBalanceAutoOnce() {
        err = camera->GetFeatureByName("BalanceWhiteAuto", balanceWhiteAutoFeature);
        if (err == VmbErrorSuccess) {
            balanceWhiteAutoFeature->SetValue("Once");
            return true;

        }
        return false;
    }
   
    void SetFrameObserver() {
        frameObserver = new FrameObserver(camera);
        observerPtr = IFrameObserverPtr(frameObserver);
    }

    // Calculate proper buffer size based on pixel format
    bool CalculateBufferSize() {
        if (camera->GetFeatureByName("PayloadSize", payloadSizeFeature) == VmbErrorSuccess) {
            payloadSizeFeature->GetValue(payloadSize);
            std::cout << "Payload size: " << payloadSize << " bytes" << std::endl;
            return true;
        }
        
        payloadSize = width * height * 3; // Default assumption
        return false;
    }

    bool PrepareFrame() {
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

    bool GetReverseFeatureToPtr() {

        err=camera->GetFeatureByName("ReverseX", imageReverseXFeature);
        if (err != VmbErrorSuccess) {
            std::cout << "Failed to get ReverseX Feature\n";
            return false;
        }
        err= camera->GetFeatureByName("ReverseY", imageReverseYFeature);
        if (err != VmbErrorSuccess) {
            std::cout << "Failed to get ReverseY Feature\n";
            return false;
        }
        return true;
    }

    bool SetTriggerOff() {
        err= camera->GetFeatureByName("TriggerMode", triggerMode);
        if (err != VmbErrorSuccess) {
            std::cout << "Failed to get trigger mode Feature\n";
            return false;
        }

        return triggerMode->SetValue("Off")==VmbErrorSuccess;
        
    }
    bool GetFrameRateFeatureToPtr() {
        err = camera->GetFeatureByName("AcquisitionFrameRate", framerateControlFeature);
        if(err!= VmbErrorSuccess){
            std::cout << "error getting framerate control feature" << std::endl;
            return false;
        }
        return true;
    }

    bool StartAcquisition() {
        // Start acquisition
        err = camera->GetFeatureByName("AcquisitionStop", acquisitionStopFeature);
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

    bool StopAcquisitionAndSysShutdown() {
        err= camera->EndCapture();
        if (err == VmbErrorSuccess) {
            camera->Close();
            sys.Shutdown();
            return true;
        }
        std::cout << "Failed to stop acquisition" << std::endl;
        return false;

    }

    bool IsRecordingVideoActive()  {
        std::lock_guard<std::mutex> lock(m_recordingMutex);
        return recordingVideo;
    }

    bool InitializeCamera() {
        CameraStartup();
        if (!GetCamera()) {
            emit CameraNotFound();
            return false;
        }
        if (!GetCameraInfo()) {
            emit CameraNotFound();
            return false;

        }

        std::cout << "yolo 1\n";
        SetPixelFormat();
        //if (!SetPixelFormat()) return false;
        std::cout << "yolo 2\n";
        if (!SetFrameDimensions()) return false;
        std::cout << "yolo 3\n";
        if (!SetWhiteBalanceAutoOnce()) return false;
        std::cout << "yolo 4\n";

        SetFrameObserver();
        std::cout << "yolo 5\n";

        if (!CalculateBufferSize()) return false;
        std::cout << "yolo 6\n";
        if (!PrepareFrame()) return false;
        std::cout << "yolo 7\n";
        if (!GetSaturationAndExposureTimeFeaturePtr()) return false;
        std::cout << "yolo 8\n";
        if (!GetReverseFeatureToPtr()) return false;
        std::cout << "yolo 9\n";
        if (!SetTriggerOff()) return false;std::cout << "yolo 10\n";
        if (!GetFrameRateFeatureToPtr()) return false;
        std::cout << "yolo 11\n";

        std::cout << "initialization success\n";

        return true;
    }



public :
    CameraInputWorker() : sys(VmbSystem::GetInstance()),frames(3){

    }

    ~CameraInputWorker() {
        //stopAcquisitionAndSysShutdown();
        
    }



public slots:
    void StartCamera() {
        if (!InitializeCamera()) return;

        if (!StartAcquisition()) return;

        emit CameraReady();

        std::cout << "starting Display Frame\n";
        GetDisplayFrame();

        

    }


signals:

    void ImageReceived(const QImage& image);
    void CameraNotFound();
    void CameraReady();
    void SendImageToCapture(const QImage& image);

public:
    


   

    void SetSaturationValue(const double& value) {
        double valueToSend = value / 100.0;
        std::cout << "value to send : " << valueToSend << '\n';

        saturationFeature->SetValue(valueToSend);

        double receivedValue;
        saturationFeature->GetValue(receivedValue);
        std::cout << "received saturaation level:  " << receivedValue << std::endl;

    }
    //Used for 0 and 180 degrees only 
    //90 and 270 degrees need to be done using software rotation
    void setImageRotation(const int& value) {
        switch (value) {
        case ANGLE_180:
            imageReverseXFeature->SetValue(true);
            imageReverseYFeature->SetValue(true);
            break;


        case ANGLE_NULL:
            imageReverseXFeature->SetValue(false);
            imageReverseYFeature->SetValue(false);
            break;



        }
    }

    void SetExposureTimeValue(const double& value) {
        std::cout << "yolo 2 " << std::endl;
        exposureFeature->SetValue(value);
        std::cout << "yolo 3 " << std::endl;
        double receivedValue;
        exposureFeature->GetValue(receivedValue);
        std::cout << "received  exposure time   :  " << receivedValue << '\n';

        framerateControlFeature->GetValue(framerate);

        std::cout << "framerate:::::::" << framerate << '\n';

    }

    public slots:

        void StartRecording() {
            std::lock_guard<std::mutex> lock(m_recordingMutex);
            recordingVideo = true;
            std::cout << "Recording started" << std::endl;
        }

        void StopRecording() {
            std::lock_guard<std::mutex> lock(m_recordingMutex);
            recordingVideo = false;
            std::cout << "Recording stopped" << std::endl;
        }


        // Display loop
        void GetDisplayFrame() {
            std::cout << "displaying frames \n";
            static QImage displayFrame;
            int frameCount = 0;
            std::cout << "entered camera get frame " << std::endl;
            bool frameToCapture = true;
            QThread* currentThread = QThread::currentThread();

            auto lastDisplayTime = std::chrono::high_resolution_clock::now();
            auto lastCaptureTime = std::chrono::high_resolution_clock::now();
            const int displayInterval = 33; // 33ms = ~30 FPS
            const int captureInterval = 40; // 40ms = 25 FPS
            const int displayWhileCaptureInterval = 50;// 40ms = 20 FPS

            while (!currentThread->isInterruptionRequested()) {
                if (frameObserver->GetFrame(displayFrame)) {
                    frameCount++;
                    if (!displayFrame.isNull()) {
                        try {
                            if (IsRecordingVideoActive()) {
 

                                // Capture at 25 FPS

                               SendFrameAtPreciseInterval(lastCaptureTime, captureInterval, [this]() { emit SendImageToCapture(displayFrame);});

                               SendFrameAtPreciseInterval(lastDisplayTime, captureInterval, [this]() { emit ImageReceived(displayFrame);});

                               
                            }
                            else {
                                // When NOT recording: limit display to 30 FPS
                                SendFrameAtPreciseInterval(lastDisplayTime, displayInterval, [this]() { emit ImageReceived(displayFrame);});

                            }
                        }
                        catch (const std::exception& e) {
                            std::cerr << "Error displaying frame: " << e.what() << std::endl;
                        }
                    }
                }

                QCoreApplication::processEvents(QEventLoop::AllEvents);

                if (IsRecordingVideoActive()) {
                    QThread::msleep(10);
                }
            }
        }
        void SendFrameAtPreciseInterval( std::chrono::steady_clock::time_point& lastUsedTime,const int& timeInterval, std::function<void()> lambda) {
            static std::chrono::steady_clock::time_point now;
            static std::chrono::milliseconds elapsedTime;
            now = std::chrono::high_resolution_clock::now();
            elapsedTime= std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUsedTime);

            if (elapsedTime.count() >= timeInterval) {
                lambda();
                lastUsedTime = now;
                
            }
            



        }

    bool CleanUpCameraRessources() {
        if (camera == nullptr) return false;


            std::cout << "=== Starting Vimba cleanup ===" << std::endl;
        try {
             //Check if objects are valid before cleanup
            std::cout << "Camera valid: " << (camera ? "YES" : "NO") << std::endl;
            std::cout << "Frames count: " << frames.size() << std::endl;

             //Stop acquisition first
            if (camera) {
                FeaturePtr stopAcquisitionFeature;
                camera->GetFeatureByName("AcquisitionStop", stopAcquisitionFeature);
                stopAcquisitionFeature->RunCommand();
                std::cout << "Stopping acquisition..." << std::endl;
               
                camera->StopContinuousImageAcquisition();
                std::cout << "Ending capture..." << std::endl;
                camera->EndCapture();
                std::cout << "Flushing queue..." << std::endl;
                camera->FlushQueue();
            }

             //Unregister observers with validation
            std::cout << "Unregistering observers..." << std::endl;
            for (size_t i = 0; i < frames.size(); ++i) {
                std::cout << "Processing frame " << i << std::endl;
                if (frames[i]) {
                    std::cout << "Frame " << i << " is valid, unregistering observer" << std::endl;
                    frames[i]->UnregisterObserver();
                    std::cout << "Frame " << i << " observer unregistered" << std::endl;
                }
                else {
                    std::cout << "Frame " << i << " is NULL" << std::endl;
                }
            }

             //Revoke frames with validation
            std::cout << "Revoking frames..." << std::endl;
            for (size_t i = 0; i < frames.size(); ++i) {
                if (frames[i] && camera) {
                    std::cout << "Revoking frame " << i << std::endl;
                    camera->RevokeFrame(frames[i]);
                    std::cout << "Frame " << i << " revoked" << std::endl;
                }
            }

             //Clear frames vector
            std::cout << "Clearing frames vector..." << std::endl;
            frames.clear();
            std::cout << "Frames vector cleared" << std::endl;

             //Close camera
            if (camera) {
                std::cout << "Closing camera..." << std::endl;
                camera->Close();
                std::cout << "Camera closed" << std::endl;

                std::cout << "Resetting camera pointer..." << std::endl;
                camera.reset(); // This might be where it crashes
                std::cout << "Camera pointer reset" << std::endl;
            }

            std::cout << "Shutting down system..." << std::endl;
            sys.Shutdown();
            std::cout << "=== Vimba cleanup complete ===" << std::endl;

        }
        catch (const std::exception& e) {
            std::cout << "Exception during cleanup: " << e.what() << std::endl;
        }
        catch (...) {
            std::cout << "Unknown exception during cleanup" << std::endl;
        }
       return true;
    }



};