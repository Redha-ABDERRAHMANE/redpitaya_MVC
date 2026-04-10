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
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <unordered_map>
#include <array>
#include "controller.hpp"
#include "commonValues.h"
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


signals:
    void PressedButtonDirection(ButtonCombination button_combination);
    void PressedFrequencyChange(const int& frequency_value);
    void PressedPhaseChange(const int& card, const int& phase_value);
    void PressedSaturationChange(const int saturation_value);
    void PressedExposureTimeChange(const int exposureTime_value);
    void InitializeMVCModel(bool state);
    void PressedRetryButton(bool state);
    void CameraRetryButtonPressed();
    void ProgramShutdown();
    void GUIReady();
    void StartCameraRecord();
    void StopCameraRecord();

    void PressedLinearStageControlButton(LinearStageAxis axis,LinearStageMotion motion);
    void PressedDimensionChange(const int dimension_index, const bool GUI_button);



private slots:
    void SelectCam();



public slots:
    void LoadControllerImage(const int& button_value);
    void HandleInputReceived(const int& button_value, const int& directionIndex);
    void HandleRecordInputReceived(const bool start_recording);
    void ConnectionToBoardsFailedPopUp();
    void CameraFailedPopUp();

    void StartGUIComponentsInitialization();

    void SetNewFrameToDisplay(const QImage& image);

    void HandleVideoRecordButton();
    void EnableLinearStageButtons();
    void SetDirectionDimension(const int& button_value, const bool GUI_button = false);
    void UpdateLinearStageMotionControl(const bool state);


private:

Ui::View*                ui                          = nullptr;
int8_t                   indexLastDirectionUsed     = -1;
double                   imageRotationAngle         = 0.0;
QList<QCameraDevice>     cameras;
QComboBox*               comboBox                   = nullptr;
//////////////////
QGraphicsView*           imageView                  = nullptr;
QGraphicsPixmapItem*     pixmapItem                 = nullptr;
QGraphicsScene*          scene                      = nullptr;
//////////
QVideoWidget*            videoWidget                = nullptr;
QCamera*                 currentCam                 = nullptr;
QMediaCaptureSession*    mediaCaptureSession        = nullptr;
////////////////////////
QGroupBox*               groupBoxCamera             = nullptr;
QGroupBox*               groupBoxInformation        = nullptr;
QVBoxLayout*             layoutverticalCamera       = nullptr;
QVBoxLayout*             layoutInformation          = nullptr;
QVBoxLayout*             layoutLeftPart             = nullptr;
///////////
QGroupBox*               controllerGroupBox         = nullptr;
QHBoxLayout*             horizontalControllerLayout = nullptr;
QGroupBox*               directionGroupBox          = nullptr;
QGridLayout*             directionGridLayout        = nullptr;
QGroupBox*               movementInfoGroupBox       = nullptr;
QVBoxLayout*             movementInfoVerticalLayout = nullptr;
QHBoxLayout*             axisInfoHorizontalLayout   = nullptr;
QHBoxLayout*             directionIndicatorHorizontalLayout = nullptr;


//////////



QLabel*                  labelControllerImage       = nullptr;
QPixmap                  pixmap;
QPushButton*             buttonDirectionPrimary     = nullptr;
QPushButton*             buttonDirectionSecondary   = nullptr;
QPushButton*             buttonDisplayCameraWindowPopup = nullptr;
//Second window widgets
// Frequency controls
QSlider*                 SliderFrequency            = nullptr;
QLabel*                  labelFrequencyValue        = nullptr;
QPushButton*             buttonFrequencyConfirmation = nullptr;
// Phase controls
QLabel*                  labelPrimaryPhaseValue     = nullptr;
QLineEdit*               textBoxPrimaryPhase        = nullptr;
QPushButton*             buttonPrimaryPhaseConfirmation = nullptr;
QLabel*                  labelSecondaryPhaseValue   = nullptr;
QLineEdit*               textBoxSecondaryPhase      = nullptr;
QPushButton*             buttonSecondaryPhaseConfirmation = nullptr;

// Exposure controls
QLabel*                  labelExposureTimeValue     = nullptr;
QSlider*                 sliderExposureTime         = nullptr;
QPushButton*             buttonExposureTimeConfirmation = nullptr;
// Saturation controls
QLabel*                  labelSaturationValue       = nullptr;
QSlider*                 sliderSaturation           = nullptr;
QPushButton*             buttonSaturationConfirmation = nullptr;
// Image rotation controls
QLabel*                  labelRotationText          = nullptr;
QRadioButton*            button270Degrees           = nullptr;
QRadioButton*            button180Degrees           = nullptr;
QRadioButton*            button90Degrees            = nullptr;
QRadioButton*            button0Degrees             = nullptr;
// Capture Video button
QPushButton*             buttonCaptureVideo         = nullptr;
bool                     recording                  = false;
// Linear Stage controls
QGroupBox*               groupBoxLinearStageControls= nullptr;
std::array<QPushButton*, LinearStageMotion::MOTIONSIZE> arrayLinearStageControlsButtons = { nullptr};
std::array<QPushButton*, (int)LinearStageAxis::AXISSIZE> arrayLinearStageAxis= { nullptr};
int currentLinearStageAxis = LinearStageAxis::XAXIS;
//Second window
QWidget*                 windowCameraPopup          = nullptr;
QVBoxLayout*             layoutCameraWindowPopup    = nullptr;

    bool cameraPopupRunning = false;
    bool popupFirstOpening = true;



    //Template for arrayDirectionButtons = {up-left, up-right,down-left, down-right, right-up , right-down, left-up  , left-down}

    std::array<QPushButton*, 10> arrayDirectionButtons = { nullptr };
    std::array<QPushButton*, 3> arrayAxisButtons = { nullptr };






    const QString imagePathXYArrows = "Images/arrows_images/arrows_XY/arrow";
    const QString imagePathGamepad = "Images/controller_images/controller";
    const std::array<ButtonCombination, 10> array_button_combination = { {
        {Buttons::HAT_UP, -1}, {Buttons::X,Buttons::HAT_UP},
        {Buttons::HAT_DOWN, -1},{Buttons::X,Buttons::HAT_DOWN},
        {Buttons::HAT_RIGHT, -1},{Buttons::Y,Buttons::HAT_RIGHT},
        {Buttons::HAT_LEFT, -1},{Buttons::Y,Buttons::HAT_LEFT},
        {Buttons::BUMPER_RIGHT, -1},{Buttons::BUMPER_LEFT, -1}
    } };

    const std::array<QString, 10> arrayXYArrowPath = {
        "_up_right.png","_up_left.png",
        "_down_right.png","_down_left.png",
        "_right_down.png","_right_up.png",
        "_left_down.png","_left_up.png",
        "_circle.png","_stop.png"
    };
    const std::unordered_map<int, QString> dictionaryControllerButtonsPath = {
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

    





private:

    void GetCameras();
    void UpdateClickedDirectionButton(const int& directionIndex);
    void UpdateLastDirectionButtonUsed(const int& newDirectionIndex);

    void SetDirectionButtons(QGridLayout*& directionGridLayout);
    void SetAxisDirectionButtons(QVBoxLayout*& movementInfoVerticalLayout, QHBoxLayout*& axisInfoHorizontalLayout, QHBoxLayout*& directionIndicatorHorizontalLayout);

    void UpdateDirectionIndicators(const int& newDirectionIndex);

    void ConfigureInfoLayout();
    void ConfigureLeftLayout();

    void ConfigureLinearStageSubLayout();

    void UpdateLinearStageButton(const LinearStageMotion motionButtonIndex);
    
   

   

protected:
    void closeEvent(QCloseEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;



};
#endif // VIEW_H