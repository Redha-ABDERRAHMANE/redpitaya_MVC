#ifndef VIEW_H
#define VIEW_H
#include <QMainWindow>
#include <QCamera>
#include <QCameraDevice>        // Changed: was QCameraDevices
#include <QMediaDevices>
#include <QVideoWidget>
#include <QMediaCaptureSession>
#include <QComboBox>
#include <QLabel>
#include <Qpixmap>
#include <QString>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QTimer>
#include <QIcon>
#include <QSlider>
#include <QGridLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QMessageBox>
#include <QCloseEvent>
#include <QRadioButton>
#include <unordered_map>
#include <array>
#include "controller.hpp"
#define withinInterval(a,b,c) (((a)<=(b)) && ((b)<=(c)))
QT_BEGIN_NAMESPACE
namespace Ui {
    class View;
}
QT_END_NAMESPACE

class View : public QMainWindow
{
    Q_OBJECT
public:
    View(QWidget* parent = nullptr);
    ~View();

    QList<QCameraDevice> cameras;
    QComboBox* comboBox;

    //////////////////
    QImage ImageToDisplay;
    QLabel ImageLabelDisplay;

    //////////
    QVideoWidget* videoWidget;


    QCamera* currentCam;


    QMediaCaptureSession* mediaCaptureSession;
    ////////////////////////

    QLabel* controllerImageLabel;
    QPixmap pixmap;
    QPushButton* testButton;
    QPushButton* button_Direction_Primary;
    QPushButton* button_Direction_Secondary;

    // Frequency controls
    QSlider* frequencySlider;
    QLabel* display_FrequencyValue;
    QPushButton* button_frequencyConfirmation;

    // Phase controls
    QLabel* display_primaryPhaseValue;
    QLineEdit* textBox_primaryPhaseValue;
    QPushButton* button_primaryPhaseConfirmation;
    QLabel* display_secondaryPhaseValue;
    QLineEdit* textBox_secondaryPhaseValue;
    QPushButton* button_secondaryPhaseConfirmation;

    // Exposure controls
    QLabel* display_ExposureTimeValue;
    QSlider* exposureTimeSlider;
    QPushButton* button_exposureTimeConfirmation;

    // Saturation controls
    QLabel* display_SaturationValue;
    QSlider* saturationSlider;
    QPushButton* button_saturationConfirmation;

    // Image rotation controls
    QLabel* display_rotationValue;
    QRadioButton* button_180DegreeRotation;
    QRadioButton* button_90DegreeRotation;
    QRadioButton* button_0DegreeRotation;


signals:
    void buttonDirection_pressed(ButtonCombination button_combination);
    void frequencyChange_pressed(const int& frequency_value);
    void phaseChange_pressed(const int& card, const int& phase_value);
    void saturationChange_pressed(const int saturation_value);
    void exposureTimeChange_pressed(const int exposureTime_value);
    void initialize_MVCModel(bool state);
    void retryButton_pressed(bool state);
    void programShutdown();
    void GUIReady();


private slots:
    void selectCam();



public slots:
    void load_ControllerImage(const int& button_value);
    void handleInputReceived(const int& button_value, const int& directionIndex);
    void connectionFailedPopUp();
    void trigger_initialization();

    void get_refresh_imageReceived(const QImage& image);

private:

    Ui::View* ui;
    int8_t last_DirectionIndexUsed = -1;



    void getCameras();
    void update_DirectionButton_clicked(const int& directionIndex);
    void update_lastDirectionButtonUsed(const int& newDirectionIndex);

    void set_Direction_buttons(QGridLayout*& directionGridLayout);
    void set_Axis_Direction_buttons(QVBoxLayout*& movementInfoVerticalLayout, QHBoxLayout*& axisInfoHorizontalLayout, QHBoxLayout*& directionIndicatorHorizontalLayout);

    void update_directionIndicators(const int& newDirectionIndex);


    //Template for array_directionButtons = {up-left, up-right,down-left, down-right, right-up , right-down, left-up  , left-down}

    std::array<QPushButton*, 10> array_directionButtons;
    std::array<QPushButton*, 3> array_axisButtons;






    const QString arrowImagePath = "C:/Users/Redha/Documents/cnrs/redpitaya_GUI/Images/arrows_images/arrow";
    const QString controllerImagePath = "C:/Users/Redha/Documents/cnrs/redpitaya_GUI/Images/controller_images/controller";
    std::array<ButtonCombination, 10> array_button_combination = { {
        {Buttons::HAT_UP, -1}, {Buttons::X,Buttons::HAT_UP},
        {Buttons::HAT_DOWN, -1},{Buttons::X,Buttons::HAT_DOWN},
        {Buttons::HAT_RIGHT, -1},{Buttons::Y,Buttons::HAT_RIGHT},
        {Buttons::HAT_LEFT, -1},{Buttons::Y,Buttons::HAT_LEFT},
        {Buttons::BUMPER_RIGHT, -1},{Buttons::BUMPER_LEFT, -1}
    } };

    const std::array<QString, 10> array_arrowPath = {
        "_up_right.png","_up_left.png",
        "_down_right.png","_down_left.png",
        "_right_down.png","_right_up.png",
        "_left_down.png","_left_up.png",
        "_circle.png","_stop.png"
    };
    const std::unordered_map<int, QString> dictionary_controllerButtonsPath = {
        {-1    , ".png"},
        {Buttons::A,"_button_A.png"},
        {Buttons::B,"_button_B.png"},
        {Buttons::X,"_button_X.png"},
        {Buttons::Y,"_button_Y.png"},

        {Buttons::HAT_UP,"_hat_up.png"},
        {Buttons::HAT_DOWN,"_hat_down.png"},
        {Buttons::HAT_LEFT,"_hat_left.png"},
        {Buttons::HAT_RIGHT,"_hat_right.png"},

        {Buttons::BUMPER_LEFT,"_LB.png"},
        {Buttons::BUMPER_RIGHT,"_RB.png"}



    };

protected:
    void closeEvent(QCloseEvent* event) override;



};
#endif // VIEW_H