#include "view.h"
#include "ui_view.h"


View::View(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::View)
{
    static int j = 0;
    std::cout << "called J: " << ++j << std::endl;

}

void View::StartGUIComponentsInitialization() {
    ui->setupUi(this);

    std::cout << "entered" << std::endl;
    QScreen* screen = QApplication::primaryScreen();
    QRect availableGeometry = screen->availableGeometry();
    setFixedSize(availableGeometry.size());
    move(availableGeometry.topLeft());







    QHBoxLayout* mainLayout = new QHBoxLayout(ui->centralwidget);


    ConfigureLeftLayout();
    ConfigureInfoLayout();
   





    /////////////////////////////////////////////////////////////////
    // Assemble left side (camera on top, controller below)
    layoutLeftPart->addWidget(groupBoxCamera, 2);
    layoutLeftPart->addWidget(controllerGroupBox, 1);

    // Add left side and right side to main layout
    mainLayout->addLayout(layoutLeftPart, 3);
    mainLayout->addWidget(groupBoxInformation, 1);


    //getCameras();
    //connect(comboBox, &QComboBox::currentIndexChanged, this, &View::selectCam);
    qDebug() << " view ready";

 
    
    emit GUIReady();


}



void View::GetCameras(){
    comboBox->addItem("<None>");
    cameras = QMediaDevices::videoInputs();
    for(const QCameraDevice& camera:std::as_const(cameras)){
        comboBox->addItem(camera.description());

    }
}

void View::UpdateClickedDirectionButton(const int& directionIndex)
{
    UpdateLastDirectionButtonUsed(directionIndex);
    UpdateDirectionIndicators(directionIndex);
    emit PressedButtonDirection( array_button_combination.at(directionIndex));
    qDebug()<< "sent to controller : current button: "<< array_button_combination.at(directionIndex).currentHat;
    qDebug()<< "sent to controller : next button: "<< array_button_combination.at(directionIndex).nextButton;


}



void View::UpdateLastDirectionButtonUsed(const int &newDirectionIndex)
{

    if(indexLastDirectionUsed != -1){
        auto previous_button=arrayDirectionButtons[indexLastDirectionUsed];
        previous_button->setDisabled(false);
        previous_button->setStyleSheet("QPushButton { background-color: #3c3c3c; } QPushButton:hover { background-color: #5c5c5c; }");
    }
    auto new_button =arrayDirectionButtons[newDirectionIndex];
    new_button->setDisabled(true);
    new_button->setStyleSheet("background-color: green");



    indexLastDirectionUsed= newDirectionIndex;



}

void View::SelectCam(){
    if(currentCam->isActive()){
        currentCam->stop();
    }
    for( const QCameraDevice& camera: std::as_const(cameras)){
        if(camera.description()==comboBox->currentText()){

            if(!currentCam || currentCam->cameraDevice() != camera){

                if(currentCam){
                    delete currentCam;
                }

                currentCam = new QCamera(camera, this);
            }

            currentCam->setCameraDevice(camera);

            mediaCaptureSession->setCamera(currentCam);
            mediaCaptureSession->setVideoOutput(videoWidget);

            currentCam->start();

            qDebug()<<"selected cam: "<<camera.description();
            return ;

        }
    }

}

void View::LoadControllerImage(const int &button_value = 22528)
{
    bool success = pixmap.load(imagePathGamepad + dictionaryControllerButtonsPath.at(button_value));
    if (success) {
        labelControllerImage->setPixmap(pixmap);
        labelControllerImage->update();
        labelControllerImage->repaint();
    }
    if(button_value != -1) {
        QTimer::singleShot(300, this, [this]() {
            LoadControllerImage(-1);
        });
    }

}


void View::SetDirectionButtons(QGridLayout*& directionGridLayout){


    uint8_t row;
    uint8_t col;




    size_t size= arrayDirectionButtons.size();


    for(size_t index = 0; index !=size; index++) {
        arrayDirectionButtons[index] = new QPushButton("&Test", this);
        connect(arrayDirectionButtons[index], &QPushButton::clicked, this, [this, index]() {
            UpdateClickedDirectionButton((const int)index);
        });
    }



    size_t array_size= arrayDirectionButtons.size();
    for(int i=0 ; i!=array_size;i++){
        arrayDirectionButtons[i]->setIcon(QIcon(imagePathXYArrows+ arrayXYArrowPath[i]));
        arrayDirectionButtons[i]->setIconSize(QSize(32,32));
        arrayDirectionButtons[i]->setText("");
        row = i / 2;  // 2 buttons per row
        col = i % 2;  // Alternate columns
        directionGridLayout->addWidget(arrayDirectionButtons[i], row, col);


    }
}

void View::SetAxisDirectionButtons(QVBoxLayout*& movementInfoVerticalLayout,QHBoxLayout*& axisInfoHorizontalLayout, QHBoxLayout*& directionIndicatorHorizontalLayout){
    /// For directionInfo
    QLabel* axisLabel = new QLabel("Axis :");
    QLabel* currentDirectionLabel = new QLabel("Direction: ");
    buttonDirectionPrimary = new QPushButton("Primary");
    buttonDirectionSecondary = new QPushButton("Secondary");

    movementInfoVerticalLayout->addWidget(axisLabel);
    movementInfoVerticalLayout->addLayout(axisInfoHorizontalLayout);
    movementInfoVerticalLayout->addWidget(currentDirectionLabel);
    movementInfoVerticalLayout->addLayout(directionIndicatorHorizontalLayout);





    // For axisinfo
    arrayAxisButtons= {
        new QPushButton("XY", this),
        new QPushButton("XZ", this),
        new QPushButton("YZ", this),
    };

    for (size_t i = 0;i < arrayAxisButtons.size();i++) {
        axisInfoHorizontalLayout->addWidget(arrayAxisButtons[i]);
        connect(arrayAxisButtons[i], &QPushButton::clicked, this, [this, i]() { 
            SetDirectionDimension(i,true);
            emit PressedDimensionChange(i,true);
            });
    }
    arrayAxisButtons[0]->setDisabled(true);
    arrayAxisButtons[0]->setStyleSheet("background-color: green");


    buttonDirectionPrimary->setDisabled(true);
    buttonDirectionSecondary->setDisabled(true);
    directionIndicatorHorizontalLayout->addWidget(buttonDirectionPrimary);
    directionIndicatorHorizontalLayout->addWidget(buttonDirectionSecondary);



}

void View::UpdateDirectionIndicators(const int &newDirectionIndex)
{
    buttonDirectionPrimary->setStyleSheet("background-color: #3c3c3c");
    buttonDirectionSecondary->setStyleSheet("background-color: #3c3c3c");


    if((newDirectionIndex & 0x1) ==1){buttonDirectionSecondary->setStyleSheet("background-color: green");}
    else{buttonDirectionPrimary->setStyleSheet("background-color: green");}
}

/* 2 roles:
 * -change controller input image
 * -disable the direction button used by the user
*/
void View::HandleInputReceived(const int& button_value , const int& directionIndex)
{
    qDebug()<<"button value: "<< button_value<<" direction : "<< directionIndex;
    LoadControllerImage(button_value);
    UpdateLastDirectionButtonUsed(directionIndex);
    UpdateDirectionIndicators(directionIndex);





}

void View:: ConnectionToBoardsFailedPopUp() {
    static int i = 0;
    std::cout << "called: " <<++i<< std::endl;
    // With custom buttons
    QMessageBox msgBox;
    msgBox.setWindowTitle("Connection Failed");
    msgBox.setText("Could not connect to SCPI server.One or multiple Red Pitaya boards are not connected (The boards needs to be connected to power and connected to the same network as the pc)");
    msgBox.setInformativeText("Would you like to retry?");
    msgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Retry);

    int result = msgBox.exec();


    if (result == QMessageBox::Retry) {
        msgBox.close();
        emit PressedRetryButton(true);


    }
    else if (result == QMessageBox::Cancel){
        msgBox.close();
        emit ProgramShutdown();
        this->close();
        
    }
}
void View:: CameraFailedPopUp() {
    static int i = 0;
    std::cout << "called: " <<++i<< std::endl;
    // With custom buttons
     QMessageBox msgBox;
    msgBox.setWindowTitle("Camera connection failed");
    msgBox.setText("Could not connect to camera. Check if the camera is connected to PC");
    msgBox.setInformativeText("Would you like to retry?");
    msgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Retry);
    msgBox.show();

    int result = msgBox.exec();


    if (result == QMessageBox::Retry) {
        msgBox.close();
        emit CameraRetryButtonPressed();


    }
    else if (result == QMessageBox::Cancel){
        msgBox.close();
        
    }
}

void View::SetNewFrameToDisplay( const QImage& image) {
    // FPS Counter - Static variables for performance tracking
    static auto lastTime = std::chrono::high_resolution_clock::now();
    static int frameCount = 0;
    static double currentFPS = 0.0;
    static const int FPS_UPDATE_INTERVAL = 30; // Update FPS display every 30 frames

    // Early return check
    if ((cameraPopupRunning && (windowCameraPopup->isMinimized())) ||
        (!cameraPopupRunning && !this->isActiveWindow())) {
        return;
    }

    // FPS Calculation
    frameCount++;
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime);

    // Update FPS every FPS_UPDATE_INTERVAL frames or every 1000ms (whichever comes first)
    if (frameCount >= FPS_UPDATE_INTERVAL || elapsed.count() >= 1000) {
        if (elapsed.count() > 0) {
            currentFPS = (frameCount * 1000.0) / elapsed.count();

            // Optional: Print FPS to console (remove in production)
            std::cout << "Display FPS: " << std::fixed 
                << currentFPS << " (" << frameCount << " frames in "
                << elapsed.count() << "ms)" << std::endl;

            // Reset counters
            frameCount = 0;
            lastTime = currentTime;
        }
    }

  
    static QSizeF imageSize = image.size();
    static QSizeF targetSize;
    static bool needResize = false;
    static qreal scale;


  if ((cameraPopupRunning &&( windowCameraPopup->isMinimized())) ||( !cameraPopupRunning && !this->isActiveWindow())) { return; }

    targetSize = imageView->viewport()->size();
    scale = qMin(targetSize.width() / imageSize.width(),targetSize.height() / imageSize.height());


    imageView->resetTransform();
    pixmapItem->resetTransform();


    pixmapItem->setPixmap(QPixmap::fromImage(image));
    




    pixmapItem->setTransform(QTransform::fromScale(scale, scale));
    pixmapItem->setRotation(imageRotationAngle);
    //imageView->centerOn(pixmapItem);
    imageView->fitInView(pixmapItem, Qt::KeepAspectRatio);
    imageView->viewport()->update();
    



    
    
    


}


void View::EnableLinearStageButtons() {
    for (int i = 0; i < LinearStageMotion::MOTIONSIZE;i++) {
        arrayLinearStageControlsButtons[i]->setDisabled(false);
    }
}

void View::SetDirectionDimension(const int& button_value, const bool GUI_button) {
    int index = 0;
    for (size_t i = 0;i < arrayAxisButtons.size();i++) {
        if (!arrayAxisButtons[i]->isEnabled()) {
            index = i;
            arrayAxisButtons[i]->setDisabled(false);
            arrayAxisButtons[index]->setStyleSheet("QPushButton { background-color: #3c3c3c; } QPushButton:hover { background-color: #5c5c5c; }");
            break;
        }

    }
    if (!GUI_button) {
        switch (button_value) {

        case Buttons::TRIGGER_LEFT:
            index = ((index - 1) % Dimensions::DIMENSIONSIZE) < 0 ? Dimensions::DIMENSIONSIZE - 1 : (index - 1);
            break;

        case Buttons::TRIGGER_RIGHT:
            index = (index + 1) % Dimensions::DIMENSIONSIZE;
            break;
        }
    }
    else { index = button_value; }



   

    arrayAxisButtons[index]->setDisabled(true);
        
    
    arrayAxisButtons[index]->setStyleSheet("background-color: green");

}

void View::ConfigureLeftLayout() {

    imageView = new QGraphicsView(this);
    scene = new QGraphicsScene(this);
    pixmapItem = new QGraphicsPixmapItem();
    imageView->installEventFilter(this);
    scene->addItem(pixmapItem);
    imageView->setScene(scene);
    imageView->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    imageView->setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    imageView->setDragMode(QGraphicsView::NoDrag);
    //////////////////////DEBUG
    imageView->setOptimizationFlags(QGraphicsView::DontAdjustForAntialiasing |
        QGraphicsView::DontSavePainterState);
    imageView->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    imageView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    imageView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //////////////////////////

    // For the QGraphicsPixmapItem (your pixmapItem)
    pixmapItem->setCacheMode(QGraphicsItem::NoCache);
    // /////////////////////
    // Main horizontal layout (splits left and right)
    // Left side vertical layout
    layoutLeftPart = new QVBoxLayout();
    // Camera group box (top left)
    groupBoxCamera = new QGroupBox("Camera", this);
    layoutverticalCamera = new QVBoxLayout();
    layoutverticalCamera->addWidget(imageView, 3);
    groupBoxCamera->setLayout(layoutverticalCamera);
    // Controller group box (bottom left)
    controllerGroupBox = new QGroupBox("Controller", this);
    horizontalControllerLayout = new QHBoxLayout();
    // Controller image (left side)
    labelControllerImage = new QLabel(this);
    LoadControllerImage(-1);
    // Direction buttons group (right side)
    directionGroupBox = new QGroupBox("Directions", this);
    directionGridLayout = new QGridLayout();
    SetDirectionButtons(directionGridLayout);
    directionGroupBox->setLayout(directionGridLayout);
    /////////////////////////////////////////////
    // Axis group
    // Direction buttons group (right side)
    movementInfoGroupBox = new QGroupBox("Info", this);
    movementInfoVerticalLayout = new QVBoxLayout();
    movementInfoGroupBox->setLayout(movementInfoVerticalLayout);
    axisInfoHorizontalLayout = new QHBoxLayout();
    // Direction text group :
    directionIndicatorHorizontalLayout = new QHBoxLayout();
    SetAxisDirectionButtons(movementInfoVerticalLayout, axisInfoHorizontalLayout, directionIndicatorHorizontalLayout);
    ///////////////////////////////////////////////////////////
    // Add both to horizontal layout
    horizontalControllerLayout->addWidget(labelControllerImage, 2);
    horizontalControllerLayout->addWidget(movementInfoGroupBox, 1);
    horizontalControllerLayout->addWidget(directionGroupBox, 1);
    controllerGroupBox->setLayout(horizontalControllerLayout);
}

void View::ConfigureInfoLayout() {

    // Info group box (right side)
    groupBoxInformation = new QGroupBox("Info", this);
    layoutInformation = new QVBoxLayout();
    layoutInformation->setSpacing(0);

    // Frequency group box with no margins
    QGroupBox* frequencyGroupBox = new QGroupBox("", this);
    QVBoxLayout* frequencyLayout = new QVBoxLayout();

    frequencyLayout->setSpacing(50);
    frequencyLayout->setAlignment(Qt::AlignTop);

    // Create the slider
    SliderFrequency = new QSlider(Qt::Horizontal, this);
    SliderFrequency->setRange(0, 50);
    SliderFrequency->setTickInterval(5);
    SliderFrequency->setSingleStep(5);
    SliderFrequency->setPageStep(5);
    SliderFrequency->setTickPosition(QSlider::TicksBothSides);
    SliderFrequency->setFixedHeight(50);
    SliderFrequency->setContentsMargins(5, 5, 5, 5);
    SliderFrequency->setValue(5);

    // Create the label to be overlaid
    labelFrequencyValue = new QLabel("Frequency slider value : 5 Hz", this);
    labelFrequencyValue->setStyleSheet("font-weight: bold;font-size: 20px;");
    labelFrequencyValue->setMargin(0);
    labelFrequencyValue->setAlignment(Qt::AlignHCenter);

    // Create the button to confirm the new frequency value 
    buttonFrequencyConfirmation = new QPushButton("Confirm", this);

    connect(buttonFrequencyConfirmation, &QPushButton::clicked, this, [this]() {
        emit PressedFrequencyChange(SliderFrequency->value());
        });

    // Add both widgets to the same cell to stack them
    frequencyLayout->addWidget(labelFrequencyValue);
    frequencyLayout->addWidget(SliderFrequency);
    frequencyLayout->addWidget(buttonFrequencyConfirmation);

    frequencyGroupBox->setLayout(frequencyLayout);

    // Manual snap-to-tick implementation
    connect(SliderFrequency, &QSlider::valueChanged, [this](int value) {
        int tickInterval = 5;
        int snappedValue = ((value + tickInterval / 2) / tickInterval) * tickInterval;
        snappedValue = qBound(0, snappedValue, 50);

        if (snappedValue != value) {
            SliderFrequency->blockSignals(true);
            SliderFrequency->setValue(snappedValue);
            SliderFrequency->blockSignals(false);
        }
        });

    // Output the snapped value when user releases the slider
    connect(SliderFrequency, &QSlider::sliderReleased, [this]() {
        qDebug() << "Frequency value :" << SliderFrequency->value();
        QString messageToDisplay = "Frequency slider value : " + QString::number(SliderFrequency->value()) + " Hz";
        labelFrequencyValue->setText(messageToDisplay);
        });

    /////////////////////////////////////////////////////////////////////
    //// PHASE SLIDER
    //// phase group box with no margins
    QGroupBox* phaseGroupBox = new QGroupBox("", this);
    QVBoxLayout* phaseLayout = new QVBoxLayout();
    // Create the label to be overlaid
    labelPrimaryPhaseValue = new QLabel("Primary card phase value :", this);
    labelPrimaryPhaseValue->setStyleSheet("font-weight: bold;font-size: 20px;");
    labelPrimaryPhaseValue->setMargin(0);
    labelPrimaryPhaseValue->setAlignment(Qt::AlignHCenter);
    textBoxPrimaryPhase = new QLineEdit("0", this);
    // Create the button to confirm the new phase value 
    buttonPrimaryPhaseConfirmation = new QPushButton("Confirm", this);
    connect(buttonPrimaryPhaseConfirmation, &QPushButton::clicked, this, [this]() {
        emit PressedPhaseChange(1, textBoxPrimaryPhase->text().toInt());
        });
    // Create the label to be overlaid
    labelSecondaryPhaseValue = new QLabel("Secondary card phase value :", this);
    labelSecondaryPhaseValue->setStyleSheet("font-weight: bold;font-size: 20px;");
    labelSecondaryPhaseValue->setMargin(0);
    labelSecondaryPhaseValue->setAlignment(Qt::AlignHCenter);
    textBoxSecondaryPhase = new QLineEdit("0", this);
    // Create the button to confirm the new phase value 
    buttonSecondaryPhaseConfirmation = new QPushButton("Confirm", this);
    connect(buttonSecondaryPhaseConfirmation, &QPushButton::clicked, this, [this]() {
        emit PressedPhaseChange(2, textBoxSecondaryPhase->text().toInt());
        });
    // Add both widgets to the same cell to stack them
    phaseLayout->addWidget(labelPrimaryPhaseValue);
    phaseLayout->addWidget(textBoxPrimaryPhase);
    phaseLayout->addWidget(buttonPrimaryPhaseConfirmation);
    phaseLayout->addWidget(labelSecondaryPhaseValue);
    phaseLayout->addWidget(textBoxSecondaryPhase);
    phaseLayout->addWidget(buttonSecondaryPhaseConfirmation);
    phaseGroupBox->setLayout(phaseLayout);

    ///////////////////////////////////////////////////////////////////

    // Other group box
    QGroupBox* otherGroupBox = new QGroupBox("", this);
    QVBoxLayout* otherLayout = new QVBoxLayout();

    // === EXPOSURE TIME SLIDER ===
    // Display label for exposure time
    labelExposureTimeValue = new QLabel("Exposure Time: 5 ms", this);

    // Create exposure time slider 
    sliderExposureTime = new QSlider(Qt::Horizontal, this);
    sliderExposureTime->setRange(1, 200);
    sliderExposureTime->setValue(5);
    sliderExposureTime->setTickPosition(QSlider::TicksBelow);
    sliderExposureTime->setTickInterval(2);

    // Create the button to confirm the new exposure time value 
    buttonExposureTimeConfirmation = new QPushButton("Confirm Exposure", this);
    connect(buttonExposureTimeConfirmation, &QPushButton::clicked, this, [this]() {
        std::cout << "yolo 1 " << std::endl;
        emit PressedExposureTimeChange(sliderExposureTime->value() * 1000);
        });

    // Manual snap-to-tick implementation for exposure time
    connect(sliderExposureTime, &QSlider::valueChanged, [this](int value) {
        int tickInterval = 2;
        int snappedValue = ((value + tickInterval / 2) / tickInterval) * tickInterval;
        snappedValue = qBound(1, snappedValue, 200);
        if (snappedValue != value) {
            sliderExposureTime->blockSignals(true);
            sliderExposureTime->setValue(snappedValue);
            sliderExposureTime->blockSignals(false);
        }
        });

    // Output the snapped value when user releases the exposure time slider
    connect(sliderExposureTime, &QSlider::sliderReleased, [this]() {
        qDebug() << "Exposure Time value:" << sliderExposureTime->value();
        QString messageToDisplay = "Exposure Time: " + QString::number(sliderExposureTime->value()) + " ms";
        labelExposureTimeValue->setText(messageToDisplay);
        });

    // === SATURATION SLIDER ===
    // Display label for saturation
    labelSaturationValue = new QLabel("Saturation: 100%", this);

    // Create saturation slider (assuming range 0-100%)
    sliderSaturation = new QSlider(Qt::Horizontal, this);
    sliderSaturation->setRange(0, 300);
    sliderSaturation->setValue(100);
    sliderSaturation->setTickPosition(QSlider::TicksBelow);
    sliderSaturation->setTickInterval(10);

    // Create the button to confirm the new saturation value 
    buttonSaturationConfirmation = new QPushButton("Confirm Saturation", this);
    connect(buttonSaturationConfirmation, &QPushButton::clicked, this, [this]() {
        emit PressedSaturationChange(sliderSaturation->value());
        });

    // Manual snap-to-tick implementation for saturation
    connect(sliderSaturation, &QSlider::valueChanged, [this](int value) {
        int tickInterval = 10;
        int snappedValue = ((value + tickInterval / 2) / tickInterval) * tickInterval;
        snappedValue = qBound(0, snappedValue, 300);
        if (snappedValue != value) {
            sliderSaturation->blockSignals(true);
            sliderSaturation->setValue(snappedValue);
            sliderSaturation->blockSignals(false);
        }
        });

    // Output the snapped value when user releases the saturation slider
    connect(sliderSaturation, &QSlider::sliderReleased, [this]() {
        qDebug() << "Saturation value:" << sliderSaturation->value();
        QString messageToDisplay = "Saturation: " + QString::number(sliderSaturation->value()) + "%";
        labelSaturationValue->setText(messageToDisplay);
        });

    //Image rotation controls
    QVBoxLayout* imageRotationLayout = new QVBoxLayout();
    QHBoxLayout* imageButtonRotationLayout = new QHBoxLayout();
    labelRotationText = new QLabel("Image rotation: ", this);
    button0Degrees = new QRadioButton("0 degrees", this);
    button90Degrees = new QRadioButton("90 degrees", this);
    button180Degrees = new QRadioButton("180 degrees", this);
    button270Degrees = new QRadioButton("270 degrees", this);

    imageRotationLayout->addWidget(labelRotationText);

    imageButtonRotationLayout->addWidget(button0Degrees);
    imageButtonRotationLayout->addWidget(button90Degrees);
    imageButtonRotationLayout->addWidget(button180Degrees);
    imageButtonRotationLayout->addWidget(button270Degrees);

    imageRotationLayout->addLayout(imageButtonRotationLayout);
    button0Degrees->setChecked(true);

    connect(button0Degrees, &QRadioButton::toggled, this, [this]() { imageRotationAngle = 0.0;});
    connect(button90Degrees, &QRadioButton::toggled, this, [this]() { imageRotationAngle = 90.0;});
    connect(button180Degrees, &QRadioButton::toggled, this, [this]() { imageRotationAngle = 180.0;});
    connect(button270Degrees, &QRadioButton::toggled, this, [this]() { imageRotationAngle = 270.0;});



    //////////////////////////////////////////////////////////////////TEST CAPTURE VIDEO
    buttonCaptureVideo = new QPushButton("Record camera feed", this);

    

    connect(buttonCaptureVideo, &QPushButton::clicked, this, [this]() {
        if (!recording) {
            buttonCaptureVideo->setText("stop recording");
            recording = true;
            emit StartCameraRecord();
        }
        else {
            recording = false;
            emit StopCameraRecord();
            buttonCaptureVideo->setText("video capture");
        }
    });

 

    // Add all widgets to the layout
    otherLayout->addWidget(labelExposureTimeValue);
    otherLayout->addWidget(sliderExposureTime);
    otherLayout->addWidget(buttonExposureTimeConfirmation);
    otherLayout->addWidget(labelSaturationValue);
    otherLayout->addWidget(sliderSaturation);
    otherLayout->addWidget(buttonSaturationConfirmation);
    otherLayout->addLayout(imageRotationLayout);
    otherLayout->addWidget(buttonCaptureVideo);

    otherGroupBox->setLayout(otherLayout);
    //LinearStage layout
    ConfigureLinearStageSubLayout();
    // Final assembly
    layoutInformation->addWidget(frequencyGroupBox);
    layoutInformation->addWidget(otherGroupBox);
    layoutInformation->addWidget(phaseGroupBox);
    
    layoutInformation->addWidget(groupBoxLinearStageControls);
    groupBoxInformation->setLayout(layoutInformation);



}



void View::ConfigureLinearStageSubLayout() {

    groupBoxLinearStageControls = new QGroupBox(this);
    QVBoxLayout* layoutLinearStageControls = new QVBoxLayout();
    QHBoxLayout* layoutLinearStageMoveButtons = new QHBoxLayout();
    QHBoxLayout* layoutLinearStageJogButtons = new QHBoxLayout();

    std::array<QString, 6> arrayLetter = { "Backward","Stop","Forward","Home","Jog backward","Jog forward" };
    for (int index = 0; index < LinearStageMotion::MOTIONSIZE;index++) {
        arrayLinearStageControlsButtons[index] = new QPushButton(arrayLetter[index],this );

        switch (index) {
        case LinearStageMotion::HOME:
            connect(arrayLinearStageControlsButtons[index], &QPushButton::clicked, this, [this, index]() {

                emit PressedLinearStageControlButton((LinearStageMotion)index);

                for (QPushButton*& button: arrayLinearStageControlsButtons) {
                    button->setDisabled(true);
                }
                });

            break;

        case LinearStageMotion::MOVEFORWARD:
        case LinearStageMotion::MOVEBACKWARD:
            std::cout << "CONNECTING BUTTONS\n";
            connect(arrayLinearStageControlsButtons[index], &QPushButton::pressed, this, [this, index]() {

                emit PressedLinearStageControlButton((LinearStageMotion)index);

                });

            connect(arrayLinearStageControlsButtons[index], &QPushButton::released, this, [this, index]() {

                emit PressedLinearStageControlButton(LinearStageMotion::STOPMOTION);

                });

            break;

        default:
            connect(arrayLinearStageControlsButtons[index], &QPushButton::clicked, this, [this, index]() {

                std::cout << "button pressed \n";
                emit PressedLinearStageControlButton((LinearStageMotion)index);

                });

            break;




        }
        

        if (index <= LinearStageMotion::HOME) {
            layoutLinearStageMoveButtons->addWidget(arrayLinearStageControlsButtons[index]);
        }
        else {
            layoutLinearStageJogButtons->addWidget(arrayLinearStageControlsButtons[index]);
        }
    }



    layoutLinearStageControls->addLayout(layoutLinearStageMoveButtons);
    layoutLinearStageControls->addLayout(layoutLinearStageJogButtons);

    groupBoxLinearStageControls->setLayout(layoutLinearStageControls);

 

}

void View::UpdateLinearStageButton(const LinearStageMotion motionButtonIndex) {

    

    emit PressedLinearStageControlButton(motionButtonIndex);

}



void View::closeEvent(QCloseEvent* event)
{
    if (recording) {

        QMessageBox::StandardButton ok_reply = QMessageBox::information(
            this,
            "Exit",
            "Please stop video capture before closing the program",
            QMessageBox::Ok
        );
        event->ignore();

        (void)ok_reply; // ADDED TO REMOVE NOT REFERENCED WARNING
    }
    else {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            "Exit",
            "Are you sure you want to exit?",
            QMessageBox::Yes | QMessageBox::No
        );

        if (reply == QMessageBox::Yes) {
            event->accept();
            if (cameraPopupRunning) {
                windowCameraPopup->close();
            }
            this->close();
            emit ProgramShutdown();
            //QApplication::quit();

        }
        else {
            event->ignore();  // Cancel the close event
        }
    }
}


bool View::eventFilter(QObject* obj, QEvent* event) {
    if (obj == imageView && event->type() == QEvent::MouseButtonDblClick) {
        std::cout << "clicked\n";
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            if (popupFirstOpening) {
                std::cout << "entered first popup\n";
                windowCameraPopup = new QWidget();
                layoutCameraWindowPopup = new QVBoxLayout(windowCameraPopup);
                windowCameraPopup->setWindowTitle("Label Popup");
                windowCameraPopup->setMinimumSize(200, 150);  // Set reasonable minimum
                popupFirstOpening = false;

                windowCameraPopup->setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowTitleHint);

                


            }
            else if (cameraPopupRunning) {
                std::cout << "entered pop up running\n";
                windowCameraPopup->hide();
                layoutCameraWindowPopup->removeWidget(imageView);
                layoutverticalCamera->addWidget(imageView);
                groupBoxCamera->show();
                layoutLeftPart->update(); // Update the layout
                layoutLeftPart->activate(); // Activate layout changes
                cameraPopupRunning = false;

                return true;

            }

                groupBoxCamera->hide(); // Hide the widget
                layoutverticalCamera->removeWidget(imageView);
                layoutCameraWindowPopup->addWidget(imageView);
                layoutCameraWindowPopup->update();
                layoutCameraWindowPopup->activate();
                layoutLeftPart->update(); // Update the layout
                layoutLeftPart->activate(); // Activate layout changes

                windowCameraPopup->show(); // show popup window
                cameraPopupRunning = true;

            
        }
        return true;
    }
    return QObject::eventFilter(obj, event);
}
View::~View()
{

    // Finally delete ui (this should be last)
    if (ui) {
        delete ui;
        ui = nullptr;
    }
}