#include "view.h"
#include "ui_view.h"


View::View(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::View)
{
    ui->setupUi(this);

    std::cout << "entered"<<std::endl;
    setFixedSize(1600, 900);

    QHBoxLayout* mainLayout = new QHBoxLayout(ui->centralwidget);
    mediaCaptureSession = new QMediaCaptureSession();
    currentCam = new QCamera();
    comboBox = new QComboBox();
    videoWidget = new QVideoWidget();

    // Main horizontal layout (splits left and right)
    // Left side vertical layout
    QVBoxLayout* leftLayout = new QVBoxLayout();

    // Camera group box (top left)
    QGroupBox* cameraGroupBox = new QGroupBox("Camera", this);
    QVBoxLayout* verticalCameraLayout = new QVBoxLayout();
    verticalCameraLayout->addWidget(videoWidget, 3);
    verticalCameraLayout->addWidget(comboBox, 1);
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
    set_Axis_Direction_buttons(movementInfoVerticalLayout,axisInfoHorizontalLayout,directionIndicatorHorizontalLayout);




    ///////////////////////////////////////////////////////////

    // Add both to horizontal layout
    horizontalControllerLayout->addWidget(controllerImageLabel, 2);
    horizontalControllerLayout->addWidget(movementInfoGroupBox,1);
    horizontalControllerLayout->addWidget(directionGroupBox, 1);



    controllerGroupBox->setLayout(horizontalControllerLayout);


    // Info group box (right side)
    QGroupBox* infoGroupBox = new QGroupBox("Info", this);
    QVBoxLayout* infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(0);

    // Frequency group box with no margins
    QGroupBox* frequencyGroupBox = new QGroupBox("", this);
    QVBoxLayout* frequencyLayout = new QVBoxLayout();
    
    frequencyLayout->setSpacing(50);        
    frequencyLayout->setAlignment(Qt::AlignTop);

    // Create the slider
    QSlider* frequencySlider = new QSlider(Qt::Horizontal);
    frequencySlider->setRange(0, 50);
    frequencySlider->setTickInterval(5);
    frequencySlider->setSingleStep(5);
    frequencySlider->setPageStep(5);
    frequencySlider->setTickPosition(QSlider::TicksBothSides);
    frequencySlider->setFixedHeight(50);
    frequencySlider->setContentsMargins(5,5,5,5);
    frequencySlider->setValue(5);

    // Create the label to be overlaid
    QLabel* display_FrequencyValue = new QLabel("Frequency slider value : 5 Hz",this);
    display_FrequencyValue->setStyleSheet("font-weight: bold;font-size: 20px;");
    display_FrequencyValue->setMargin(0);
    display_FrequencyValue->setAlignment(Qt::AlignHCenter);
    


  

    // Create the button to confirm the new frequency value 
    QPushButton* button_frequencyConfirmation = new QPushButton("Confirm",this);

    connect(button_frequencyConfirmation, &QPushButton::clicked, this, [this,frequencySlider]() {
        emit frequencyChange_pressed(frequencySlider->value());
        });


    // Add both widgets to the same cell to stack them
    frequencyLayout->addWidget(display_FrequencyValue);
    frequencyLayout->addWidget(frequencySlider);
    frequencyLayout->addWidget(button_frequencyConfirmation);
    

    frequencyGroupBox->setLayout(frequencyLayout);

    // Manual snap-to-tick implementation
    connect(frequencySlider, &QSlider::valueChanged, [frequencySlider](int value) {
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
    connect(frequencySlider, &QSlider::sliderReleased, [display_FrequencyValue,frequencySlider]() {
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
    QLabel* display_primaryPhaseValue = new QLabel("Primary card phase value :", this);
    display_primaryPhaseValue->setStyleSheet("font-weight: bold;font-size: 20px;");
    display_primaryPhaseValue->setMargin(0);
    display_primaryPhaseValue->setAlignment(Qt::AlignHCenter);

    QLineEdit* textBox_primaryPhaseValue = new QLineEdit("0", this);

    // Create the button to confirm the new phase value 
    QPushButton* button_primaryPhaseConfirmation = new QPushButton("Confirm", this);

    connect(button_primaryPhaseConfirmation, &QPushButton::clicked, this, [this, textBox_primaryPhaseValue]() {
        emit phaseChange_pressed(1,textBox_primaryPhaseValue->text().toInt());
        });

    // Create the label to be overlaid
    QLabel* display_secondaryPhaseValue = new QLabel("Secondary card phase value :", this);
    display_secondaryPhaseValue->setStyleSheet("font-weight: bold;font-size: 20px;");
    display_secondaryPhaseValue->setMargin(0);
    display_secondaryPhaseValue->setAlignment(Qt::AlignHCenter);

    QLineEdit* textBox_secondaryPhaseValue = new QLineEdit("0", this);

    // Create the button to confirm the new phase value 
    QPushButton* button_secondaryPhaseConfirmation = new QPushButton("Confirm", this);

    connect(button_secondaryPhaseConfirmation, &QPushButton::clicked, this, [this, textBox_secondaryPhaseValue]() {
        emit phaseChange_pressed(2,textBox_secondaryPhaseValue->text().toInt());
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
    otherGroupBox->setLayout(otherLayout);

    // Final assembly
    infoLayout->addWidget(frequencyGroupBox);
    infoLayout->addWidget(otherGroupBox);
    infoLayout->addWidget(phaseGroupBox);
    infoGroupBox->setLayout(infoLayout);





    /////////////////////////////////////////////////////////////////
    // Assemble left side (camera on top, controller below)
    leftLayout->addWidget(cameraGroupBox, 2);
    leftLayout->addWidget(controllerGroupBox, 1);

    // Add left side and right side to main layout
    mainLayout->addLayout(leftLayout, 2);
    mainLayout->addWidget(infoGroupBox, 1);

    getCameras();
    connect(comboBox, &QComboBox::currentIndexChanged, this, &View::selectCam);

}


View::~View()
{

    delete ui;

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
