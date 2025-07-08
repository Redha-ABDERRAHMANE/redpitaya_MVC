#include "view.h"
#include "ui_view.h"


View::View(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::View)
{
    static int j = 0;
    std::cout << "called J: " << ++j << std::endl;

}

void View::trigger_initialization() {
    ui->setupUi(this);

    std::cout << "entered" << std::endl;
    QScreen* screen = QApplication::primaryScreen();
    QRect availableGeometry = screen->availableGeometry();
    setFixedSize(availableGeometry.size());
    move(availableGeometry.topLeft());







    QHBoxLayout* mainLayout = new QHBoxLayout(ui->centralwidget);
    ////////////////////////////////////////////////////////////
    //mediaCaptureSession = new QMediaCaptureSession();
    //currentCam = new QCamera();
    //comboBox = new QComboBox();
    //videoWidget = new QVideoWidget();
    // 
    
    //ImageLabelDisplay.setScaledContents(true);
    imageView = new QGraphicsView(this);
    scene = new QGraphicsScene(this);
    pixmapItem = new QGraphicsPixmapItem();
    imageView->installEventFilter(this);
    scene->addItem(pixmapItem);
    imageView->setScene(scene);
    //////////////////////////////////////////////////////////////
    // Main horizontal layout (splits left and right)
    // Left side vertical layout
    leftLayout = new QVBoxLayout();

    // Camera group box (top left)
    cameraGroupBox = new QGroupBox("Camera", this);
    verticalCameraLayout = new QVBoxLayout();
    verticalCameraLayout->addWidget(imageView, 3);
    cameraGroupBox->setLayout(verticalCameraLayout);

    // Controller group box (bottom left)
    QGroupBox* controllerGroupBox = new QGroupBox("Controller", this);
    QHBoxLayout* horizontalControllerLayout = new QHBoxLayout();

    // Controller image (left side)
    controllerImageLabel = new QLabel(this);
    load_ControllerImage(-1);

    // Direction buttons group (right side)
    QGroupBox* directionGroupBox = new QGroupBox("Directions", this);
    QGridLayout* directionGridLayout = new QGridLayout();

    set_Direction_buttons(directionGridLayout);
    directionGroupBox->setLayout(directionGridLayout);
    /////////////////////////////////////////////
    // Axis group
    // Direction buttons group (right side)
    QGroupBox* movementInfoGroupBox = new QGroupBox("Info", this);
    QVBoxLayout* movementInfoVerticalLayout = new QVBoxLayout();
    movementInfoGroupBox->setLayout(movementInfoVerticalLayout);

    QHBoxLayout* axisInfoHorizontalLayout = new QHBoxLayout();



    // Direction text group :
    QHBoxLayout* directionIndicatorHorizontalLayout = new QHBoxLayout();
    set_Axis_Direction_buttons(movementInfoVerticalLayout, axisInfoHorizontalLayout, directionIndicatorHorizontalLayout);




    ///////////////////////////////////////////////////////////

    // Add both to horizontal layout
    horizontalControllerLayout->addWidget(controllerImageLabel, 2);
    horizontalControllerLayout->addWidget(movementInfoGroupBox, 1);
    horizontalControllerLayout->addWidget(directionGroupBox, 1);



    controllerGroupBox->setLayout(horizontalControllerLayout);


    configureInfoLayout();
   





    /////////////////////////////////////////////////////////////////
    // Assemble left side (camera on top, controller below)
    leftLayout->addWidget(cameraGroupBox, 2);
    leftLayout->addWidget(controllerGroupBox, 1);

    // Add left side and right side to main layout
    mainLayout->addLayout(leftLayout, 3);
    mainLayout->addWidget(infoGroupBox, 1);

    //getCameras();
    //connect(comboBox, &QComboBox::currentIndexChanged, this, &View::selectCam);
    qDebug() << " view ready";

   
    ///////////////////////////////////////////////////////////////////////////
    QImage image("C:/Users/Redha/Downloads/nature.jfif");
    //QImage imagescaled= image.scaled(1920, 1080, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    //std::cout << "image width" << imagescaled.width() << '\n';
 
    
    
    /////////////////////////////////////////////////////////////////////////////////////////
    
    emit GUIReady();


}



void View::getCameras(){
    comboBox->addItem("<None>");
    cameras = QMediaDevices::videoInputs();
    for(const QCameraDevice& camera:std::as_const(cameras)){
        comboBox->addItem(camera.description());

    }
}

void View::update_DirectionButton_clicked(const int& directionIndex)
{
    update_lastDirectionButtonUsed(directionIndex);
    emit buttonDirection_pressed( array_button_combination.at(directionIndex));
    qDebug()<< "sent to controller : current button: "<< array_button_combination.at(directionIndex).currentHat;
    qDebug()<< "sent to controller : next button: "<< array_button_combination.at(directionIndex).nextButton;


}



void View::update_lastDirectionButtonUsed(const int &newDirectionIndex)
{

    if(last_DirectionIndexUsed != -1){
        auto previous_button=array_directionButtons[last_DirectionIndexUsed];
        previous_button->setDisabled(false);
        previous_button->setStyleSheet("background-color: #3c3c3c; QPushButton:hover { background-color: #5c5c5c; }");
    }
    auto new_button =array_directionButtons[newDirectionIndex];
    new_button->setDisabled(true);
    new_button->setStyleSheet("background-color: green");



    last_DirectionIndexUsed= newDirectionIndex;



}

void View::selectCam(){
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

void View::load_ControllerImage(const int &button_value = 22528)
{
    bool success = pixmap.load(controllerImagePath + dictionary_controllerButtonsPath.at(button_value));
    if (success) {
        controllerImageLabel->setPixmap(pixmap);
        controllerImageLabel->update();
        controllerImageLabel->repaint();
    }
    if(button_value != -1) {
        QTimer::singleShot(300, this, [this]() {
            load_ControllerImage(-1);
        });
    }

}


void View::set_Direction_buttons(QGridLayout*& directionGridLayout){


    uint8_t row;
    uint8_t col;




    size_t size= array_directionButtons.size();


    for(size_t index = 0; index !=size; index++) {
        array_directionButtons[index] = new QPushButton("&Test", this);
        connect(array_directionButtons[index], &QPushButton::clicked, this, [this, index]() {
            update_DirectionButton_clicked((const int)index);
        });
    }



    size_t array_size= array_directionButtons.size();
    for(int i=0 ; i!=array_size;i++){
        array_directionButtons[i]->setIcon(QIcon(arrowImagePath+ array_arrowPath[i]));
        array_directionButtons[i]->setIconSize(QSize(32,32));
        array_directionButtons[i]->setText("");
        row = i / 2;  // 2 buttons per row
        col = i % 2;  // Alternate columns
        directionGridLayout->addWidget(array_directionButtons[i], row, col);


    }
}

void View::set_Axis_Direction_buttons(QVBoxLayout*& movementInfoVerticalLayout,QHBoxLayout*& axisInfoHorizontalLayout, QHBoxLayout*& directionIndicatorHorizontalLayout){
    /// For directionInfo
    QLabel* axisLabel = new QLabel("Axis :");
    QLabel* currentDirectionLabel = new QLabel("Direction: ");
    button_Direction_Primary = new QPushButton("Primary");
    button_Direction_Secondary = new QPushButton("Secondary");

    movementInfoVerticalLayout->addWidget(axisLabel);
    movementInfoVerticalLayout->addLayout(axisInfoHorizontalLayout);
    movementInfoVerticalLayout->addWidget(currentDirectionLabel);
    movementInfoVerticalLayout->addLayout(directionIndicatorHorizontalLayout);





    // For axisinfo
    array_axisButtons= {
        new QPushButton("XY", this),
        new QPushButton("XZ", this),
        new QPushButton("YZ", this),
    };

    for(QPushButton*& axisButton: array_axisButtons) {

        axisButton->setDisabled(true);
        axisInfoHorizontalLayout->addWidget(axisButton);
    }
    array_axisButtons[0]->setStyleSheet("background-color: green");


    button_Direction_Primary->setDisabled(true);
    button_Direction_Secondary->setDisabled(true);
    directionIndicatorHorizontalLayout->addWidget(button_Direction_Primary);
    directionIndicatorHorizontalLayout->addWidget(button_Direction_Secondary);



}

void View::update_directionIndicators(const int &newDirectionIndex)
{
    button_Direction_Primary->setStyleSheet("background-color: #3c3c3c");
    button_Direction_Secondary->setStyleSheet("background-color: #3c3c3c");


    if((newDirectionIndex & 0x1) ==1){button_Direction_Secondary->setStyleSheet("background-color: green");}
    else{button_Direction_Primary->setStyleSheet("background-color: green");}
}

/* 2 roles:
 * -change controller input image
 * -disable the direction button used by the user
*/
void View::handleInputReceived(const int& button_value , const int& directionIndex)
{
    qDebug()<<"button value: "<< button_value<<" direction : "<< directionIndex;
    load_ControllerImage(button_value);
    update_lastDirectionButtonUsed(directionIndex);
    update_directionIndicators(directionIndex);





}

void View:: connectionFailedPopUp() {
    static int i = 0;
    std::cout << "called: " <<++i<< std::endl;
    // With custom buttons
    QMessageBox msgBox;
    msgBox.setWindowTitle("Connection Failed");
    msgBox.setText("Could not connect to SCPI server.one or multiple Red Pitaya boards are not connected (The boards needs to be connected to power and connected to the same network as the pc)");
    msgBox.setInformativeText("Would you like to retry?");
    msgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Retry);

    int result = msgBox.exec();


    if (result == QMessageBox::Retry) {
        msgBox.close();
        // Retry connection
        QMessageBox retryBox;
        retryBox.setText("Trying to connect...");
        retryBox.setStandardButtons(QMessageBox::NoButton);
        retryBox.show();
        emit retryButton_pressed(true);
        QTimer::singleShot(1000, this, [&retryBox]() {
            retryBox.close();
            });

    }
    else if (result == QMessageBox::Cancel){
        msgBox.close();
        this->close();
        emit programShutdown();
    }
}

void View::get_refresh_imageReceived( const QImage& image) {

    if ((popup_running && window_popup->isMinimized()) || (!popup_running && this->isMinimized())) { return; }
    QSize imageSize = popup_running ? window_popup->size() : cameraGroupBox->size();
    QSize temp = QSize(imageSize.width(), imageSize.height() / 2);
    std::cout << "QImage size : " << imageSize.height()<<"x "<<imageSize.width() << '\n';
    imageView->setFixedSize(imageSize);
    pixmapItem->setPixmap(QPixmap::fromImage(image).scaled(temp));
    pixmapItem->setRotation(imageRotationAngle);

    imageView->centerOn(pixmapItem);
    
    


}

void View::configureInfoLayout() {

    // Info group box (right side)
    infoGroupBox = new QGroupBox("Info", this);
    infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(0);

    // Frequency group box with no margins
    QGroupBox* frequencyGroupBox = new QGroupBox("", this);
    QVBoxLayout* frequencyLayout = new QVBoxLayout();

    frequencyLayout->setSpacing(50);
    frequencyLayout->setAlignment(Qt::AlignTop);

    // Create the slider
    frequencySlider = new QSlider(Qt::Horizontal, this);
    frequencySlider->setRange(0, 50);
    frequencySlider->setTickInterval(5);
    frequencySlider->setSingleStep(5);
    frequencySlider->setPageStep(5);
    frequencySlider->setTickPosition(QSlider::TicksBothSides);
    frequencySlider->setFixedHeight(50);
    frequencySlider->setContentsMargins(5, 5, 5, 5);
    frequencySlider->setValue(5);

    // Create the label to be overlaid
    display_FrequencyValue = new QLabel("Frequency slider value : 5 Hz", this);
    display_FrequencyValue->setStyleSheet("font-weight: bold;font-size: 20px;");
    display_FrequencyValue->setMargin(0);
    display_FrequencyValue->setAlignment(Qt::AlignHCenter);

    // Create the button to confirm the new frequency value 
    button_frequencyConfirmation = new QPushButton("Confirm", this);

    connect(button_frequencyConfirmation, &QPushButton::clicked, this, [this]() {
        emit frequencyChange_pressed(frequencySlider->value());
        });

    // Add both widgets to the same cell to stack them
    frequencyLayout->addWidget(display_FrequencyValue);
    frequencyLayout->addWidget(frequencySlider);
    frequencyLayout->addWidget(button_frequencyConfirmation);

    frequencyGroupBox->setLayout(frequencyLayout);

    // Manual snap-to-tick implementation
    connect(frequencySlider, &QSlider::valueChanged, [this](int value) {
        int tickInterval = 5;
        int snappedValue = ((value + tickInterval / 2) / tickInterval) * tickInterval;
        snappedValue = qBound(0, snappedValue, 50);

        if (snappedValue != value) {
            frequencySlider->blockSignals(true);
            frequencySlider->setValue(snappedValue);
            frequencySlider->blockSignals(false);
        }
        });

    // Output the snapped value when user releases the slider
    connect(frequencySlider, &QSlider::sliderReleased, [this]() {
        qDebug() << "Frequency value :" << frequencySlider->value();
        QString messageToDisplay = "Frequency slider value : " + QString::number(frequencySlider->value()) + " Hz";
        display_FrequencyValue->setText(messageToDisplay);
        });

    ///////////////////////////////////////////////////////////////////
    // PHASE SLIDER
    // phase group box with no margins
    QGroupBox* phaseGroupBox = new QGroupBox("", this);
    QVBoxLayout* phaseLayout = new QVBoxLayout();

    // Create the label to be overlaid
    display_primaryPhaseValue = new QLabel("Primary card phase value :", this);
    display_primaryPhaseValue->setStyleSheet("font-weight: bold;font-size: 20px;");
    display_primaryPhaseValue->setMargin(0);
    display_primaryPhaseValue->setAlignment(Qt::AlignHCenter);

    textBox_primaryPhaseValue = new QLineEdit("0", this);

    // Create the button to confirm the new phase value 
    button_primaryPhaseConfirmation = new QPushButton("Confirm", this);

    connect(button_primaryPhaseConfirmation, &QPushButton::clicked, this, [this]() {
        emit phaseChange_pressed(1, textBox_primaryPhaseValue->text().toInt());
        });

    // Create the label to be overlaid
    display_secondaryPhaseValue = new QLabel("Secondary card phase value :", this);
    display_secondaryPhaseValue->setStyleSheet("font-weight: bold;font-size: 20px;");
    display_secondaryPhaseValue->setMargin(0);
    display_secondaryPhaseValue->setAlignment(Qt::AlignHCenter);

    textBox_secondaryPhaseValue = new QLineEdit("0", this);

    // Create the button to confirm the new phase value 
    button_secondaryPhaseConfirmation = new QPushButton("Confirm", this);

    connect(button_secondaryPhaseConfirmation, &QPushButton::clicked, this, [this]() {
        emit phaseChange_pressed(2, textBox_secondaryPhaseValue->text().toInt());
        });

    // Add both widgets to the same cell to stack them
    phaseLayout->addWidget(display_primaryPhaseValue);
    phaseLayout->addWidget(textBox_primaryPhaseValue);
    phaseLayout->addWidget(button_primaryPhaseConfirmation);

    phaseLayout->addWidget(display_secondaryPhaseValue);
    phaseLayout->addWidget(textBox_secondaryPhaseValue);
    phaseLayout->addWidget(button_secondaryPhaseConfirmation);

    phaseGroupBox->setLayout(phaseLayout);

    ///////////////////////////////////////////////////////////////////

    // Other group box
    QGroupBox* otherGroupBox = new QGroupBox("", this);
    QVBoxLayout* otherLayout = new QVBoxLayout();

    // === EXPOSURE TIME SLIDER ===
    // Display label for exposure time
    display_ExposureTimeValue = new QLabel("Exposure Time: 5 ms", this);

    // Create exposure time slider 
    exposureTimeSlider = new QSlider(Qt::Horizontal, this);
    exposureTimeSlider->setRange(1, 200);
    exposureTimeSlider->setValue(5);
    exposureTimeSlider->setTickPosition(QSlider::TicksBelow);
    exposureTimeSlider->setTickInterval(2);

    // Create the button to confirm the new exposure time value 
    button_exposureTimeConfirmation = new QPushButton("Confirm Exposure", this);
    connect(button_exposureTimeConfirmation, &QPushButton::clicked, this, [this]() {
        std::cout << "yolo 1 " << std::endl;
        emit exposureTimeChange_pressed(exposureTimeSlider->value() * 1000);
        });

    // Manual snap-to-tick implementation for exposure time
    connect(exposureTimeSlider, &QSlider::valueChanged, [this](int value) {
        int tickInterval = 2;
        int snappedValue = ((value + tickInterval / 2) / tickInterval) * tickInterval;
        snappedValue = qBound(1, snappedValue, 200);
        if (snappedValue != value) {
            exposureTimeSlider->blockSignals(true);
            exposureTimeSlider->setValue(snappedValue);
            exposureTimeSlider->blockSignals(false);
        }
        });

    // Output the snapped value when user releases the exposure time slider
    connect(exposureTimeSlider, &QSlider::sliderReleased, [this]() {
        qDebug() << "Exposure Time value:" << exposureTimeSlider->value();
        QString messageToDisplay = "Exposure Time: " + QString::number(exposureTimeSlider->value()) + " ms";
        display_ExposureTimeValue->setText(messageToDisplay);
        });

    // === SATURATION SLIDER ===
    // Display label for saturation
    display_SaturationValue = new QLabel("Saturation: 100%", this);

    // Create saturation slider (assuming range 0-100%)
    saturationSlider = new QSlider(Qt::Horizontal, this);
    saturationSlider->setRange(0, 300);
    saturationSlider->setValue(100);
    saturationSlider->setTickPosition(QSlider::TicksBelow);
    saturationSlider->setTickInterval(10);

    // Create the button to confirm the new saturation value 
    button_saturationConfirmation = new QPushButton("Confirm Saturation", this);
    connect(button_saturationConfirmation, &QPushButton::clicked, this, [this]() {
        emit saturationChange_pressed(saturationSlider->value());
        });

    // Manual snap-to-tick implementation for saturation
    connect(saturationSlider, &QSlider::valueChanged, [this](int value) {
        int tickInterval = 10;
        int snappedValue = ((value + tickInterval / 2) / tickInterval) * tickInterval;
        snappedValue = qBound(0, snappedValue, 300);
        if (snappedValue != value) {
            saturationSlider->blockSignals(true);
            saturationSlider->setValue(snappedValue);
            saturationSlider->blockSignals(false);
        }
        });

    // Output the snapped value when user releases the saturation slider
    connect(saturationSlider, &QSlider::sliderReleased, [this]() {
        qDebug() << "Saturation value:" << saturationSlider->value();
        QString messageToDisplay = "Saturation: " + QString::number(saturationSlider->value()) + "%";
        display_SaturationValue->setText(messageToDisplay);
        });

    //Image rotation controls
    QVBoxLayout* imageRotationLayout = new QVBoxLayout();
    QHBoxLayout* imageButtonRotationLayout = new QHBoxLayout();
    display_rotationValue = new QLabel("Image rotation: ", this);
    button_0DegreeRotation = new QRadioButton("0 degrees", this);
    button_90DegreeRotation = new QRadioButton("90 degrees", this);
    button_180DegreeRotation = new QRadioButton("180 degrees", this);
    button_270DegreeRotation = new QRadioButton("270 degrees", this);

    imageRotationLayout->addWidget(display_rotationValue);

    imageButtonRotationLayout->addWidget(button_0DegreeRotation);
    imageButtonRotationLayout->addWidget(button_90DegreeRotation);
    imageButtonRotationLayout->addWidget(button_180DegreeRotation);
    imageButtonRotationLayout->addWidget(button_270DegreeRotation);

    imageRotationLayout->addLayout(imageButtonRotationLayout);
    button_0DegreeRotation->setChecked(true);

    connect(button_0DegreeRotation, &QRadioButton::toggled, this, [this]() { imageRotationAngle = 0.0;});
    connect(button_90DegreeRotation, &QRadioButton::toggled, this, [this]() { imageRotationAngle = 90.0;});
    connect(button_180DegreeRotation, &QRadioButton::toggled, this, [this]() { imageRotationAngle = 180.0;});
    connect(button_270DegreeRotation, &QRadioButton::toggled, this, [this]() { imageRotationAngle = 270.0;});

    // Add all widgets to the layout
    otherLayout->addWidget(display_ExposureTimeValue);
    otherLayout->addWidget(exposureTimeSlider);
    otherLayout->addWidget(button_exposureTimeConfirmation);
    otherLayout->addWidget(display_SaturationValue);
    otherLayout->addWidget(saturationSlider);
    otherLayout->addWidget(button_saturationConfirmation);
    otherLayout->addLayout(imageRotationLayout);

    otherGroupBox->setLayout(otherLayout);

    // Final assembly
    infoLayout->addWidget(frequencyGroupBox);
    infoLayout->addWidget(otherGroupBox);
    infoLayout->addWidget(phaseGroupBox);
    infoGroupBox->setLayout(infoLayout);



}



void View::closeEvent(QCloseEvent* event)
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Exit",
        "Are you sure you want to exit?",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        event->accept();
        if (popup_running) {
            window_popup->close();
        }
        this->close();
        emit programShutdown();
        //QApplication::quit();
       
    }
    else {
        event->ignore();  // Cancel the close event
    }
}


bool View::eventFilter(QObject* obj, QEvent* event) {
    if (obj == imageView && event->type() == QEvent::MouseButtonDblClick) {
        std::cout << "clicked\n";
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            if (popup_firstOpening) {
                std::cout << "entered first popup\n";
                window_popup = new QWidget();
                window_popupLayout = new QVBoxLayout(window_popup);
                window_popup->setWindowTitle("Label Popup");
                window_popup->setMinimumSize(200, 150);  // Set reasonable minimum
                popup_firstOpening = false;

                window_popup->setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowTitleHint);

                


            }
            else if (popup_running) {
                std::cout << "entered pop up running\n";
                window_popup->hide();
                window_popupLayout->removeWidget(imageView);
                verticalCameraLayout->addWidget(imageView);
                cameraGroupBox->show();
                leftLayout->update(); // Update the layout
                leftLayout->activate(); // Activate layout changes
                popup_running = false;

                return true;

            }

                cameraGroupBox->hide(); // Hide the widget
                verticalCameraLayout->removeWidget(imageView);
                window_popupLayout->addWidget(imageView);
                leftLayout->update(); // Update the layout
                leftLayout->activate(); // Activate layout changes

                window_popup->show(); // show popup window
                popup_running = true;

            
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