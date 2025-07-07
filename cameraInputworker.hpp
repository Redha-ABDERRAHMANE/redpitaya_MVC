#include <iostream>
#include <VmbCPP/VmbCPP.h>
#include <QObject>
#include <QImage>
#include <QThread>
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
            catch (...) {
                std::cerr << " error in frame processing " << std::endl;
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

class CameraInputWorkerThread : public QThread {
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
    VmbInt64_t width = 640, height = 480;
    FrameObserver* frameObserver = nullptr;
    IFrameObserverPtr observerPtr;
    VmbInt64_t payloadSize = 0;

    FramePtrVector frames;
    FeaturePtr triggerMode;


    bool running = true;
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

    void configureSaturationAndExposureTimePtr() {
        camera->GetFeatureByName("ExposureTime", exposureFeature);
        camera->GetFeatureByName("Saturation", saturationFeature);
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
        widthFeature->GetValue(width);

        std::cout << "Frame size: " << width << "x" << height << std::endl;
    }
    // Create frame observer - use shared_ptr for proper memory management
    void setFrameObserver() {
        frameObserver = new FrameObserver(camera);
        observerPtr = IFrameObserverPtr(frameObserver);
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
    CameraInputWorkerThread(QObject* parent = nullptr) : sys(VmbSystem::GetInstance()),frames(3){}

    ~CameraInputWorkerThread() {
        //stopAcquisitionAndSysShutdown();
        
    }
protected :

    void run() override  {
            cameraStartup();
            getCamera();
            getCameraInfo();
            setPixelFormat();
            setFrameDimensions();
            setFrameObserver();
            calculateBufferSize();
            prepareFrame();
            configureSaturationAndExposureTimePtr();

            camera->GetFeatureByName("ReverseX", imageReverseXFeature);
            camera->GetFeatureByName("ReverseY", imageReverseYFeature);

            
            camera->GetFeatureByName("TriggerMode", triggerMode);
            
            triggerMode->SetValue("Off");




            err = camera->GetFeatureByName("AcquisitionFrameRateEnable", framerateControlEnableFeature);
            if (err == VmbErrorSuccess) {
                framerateControlEnableFeature->SetValue(false);
                std::cout << "Frame rate control enabled" << std::endl;
            }
            else {
                std::cout << "AcquisitionFrameRateEnable not available" << std::endl;
            }
            err = camera->GetFeatureByName("AcquisitionFrameRate", framerateControlFeature);
            framerateControlFeature->SetValue(30.0);
            framerateControlFeature->GetValue(framerate);

            std::cout << "framerate:::::::" << framerate << '\n';

            startAcquisition();
            getDisplayFrame();

            emit cameraReady();

    }


signals:

    void ImageReceived(const QImage& image);
    void cameraReady();

public:
    


   

    void setSaturationValue(const double& value) {
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

    void setExposureTimeValue(const double& value) {
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
        // Display loop
    void getDisplayFrame() {
        std::cout << "displaying frames \n";
        QImage displayFrame;
        int frameCount = 0;
        std::cout << "entered camera get frame " << std::endl;
        

        while (!isInterruptionRequested()) {
            if (frameObserver->GetFrame(displayFrame)) {
                frameCount++;
                if (frameCount % 30 == 0) {
                    std::cout << "Received " << frameCount << " frames" << std::endl;
                }

                if (!displayFrame.isNull()) {
                    try {

                        emit ImageReceived(displayFrame.copy());
                        
                    }
                    catch (...) {
                        std::cerr << "Error displaying frame" << std::endl;
                    }
                }
            }
            QThread::msleep(10);
        }

        std::cout << "Stopping acquisition..." << std::endl;
    }

    bool endOfWork() {


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