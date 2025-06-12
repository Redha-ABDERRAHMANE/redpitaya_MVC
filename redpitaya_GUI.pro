QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia multimediawidgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    view.cpp

HEADERS += \
    MVC_Controller.hpp \
    MVC_Model.hpp \
    RedpitayaCard.hpp \
    RpSignalGn.hpp \
    applyInputworker.h \
    commonValues.h \
    controller.hpp \
    inputworker.h \
    scpi.hpp \
    view.h \
    waveGnPresets.hpp

FORMS += \
    view.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    Images/arrows_images/arrow_circle.png \
    Images/arrows_images/arrow_down_left.png \
    Images/arrows_images/arrow_down_right.png \
    Images/arrows_images/arrow_left_down.png \
    Images/arrows_images/arrow_left_up.png \
    Images/arrows_images/arrow_right_down.png \
    Images/arrows_images/arrow_right_up.png \
    Images/arrows_images/arrow_up_left.png \
    Images/arrows_images/arrow_up_right.png \
    Images/controller_images/Controller.svg \
    Images/controller_images/controller.png \
    Images/controller_images/controller_LB.png \
    Images/controller_images/controller_LT.png \
    Images/controller_images/controller_RB.png \
    Images/controller_images/controller_RT.png \
    Images/controller_images/controller_button_A.png \
    Images/controller_images/controller_button_B.png \
    Images/controller_images/controller_button_X.png \
    Images/controller_images/controller_button_Y.png \
    Images/controller_images/controller_digi_down.png \
    Images/controller_images/controller_digi_left.png \
    Images/controller_images/controller_digi_right.png \
    Images/controller_images/controller_digi_up.png \
    Images/controller_images/controller_menu.png \
    Images/controller_images/controller_select.png \
    Images/controller_images/controller_stick_left.png \
    Images/controller_images/controller_stick_left_press_0.png \
    Images/controller_images/controller_stick_left_press_1.png \
    Images/controller_images/controller_stick_right.png \
    Images/controller_images/controller_stick_right_press_0.png \
    Images/controller_images/controller_stick_right_press_1.png \
    Images/controller_images/controller_top.png \
    Images/controller_images/spritesheet/A.png \
    Images/controller_images/spritesheet/B.png \
    Images/controller_images/spritesheet/Buttons.png \
    Images/controller_images/spritesheet/X.png \
    Images/controller_images/spritesheet/Y.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/arrows_images/arrow_circle.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/arrows_images/arrow_down_left.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/arrows_images/arrow_down_right.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/arrows_images/arrow_left_down.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/arrows_images/arrow_left_up.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/arrows_images/arrow_right_down.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/arrows_images/arrow_right_up.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/arrows_images/arrow_up_left.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/arrows_images/arrow_up_right.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/controller_images/Controller.svg \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/controller_images/controller.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/controller_images/controller_LB.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/controller_images/controller_LT.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/controller_images/controller_RB.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/controller_images/controller_RT.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/controller_images/controller_button_A.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/controller_images/controller_button_B.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/controller_images/controller_button_X.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/controller_images/controller_button_Y.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/controller_images/controller_digi_down.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/controller_images/controller_digi_left.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/controller_images/controller_digi_right.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/controller_images/controller_digi_up.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/controller_images/controller_menu.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/controller_images/controller_select.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/controller_images/controller_stick_left.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/controller_images/controller_stick_left_press_0.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/controller_images/controller_stick_left_press_1.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/controller_images/controller_stick_right.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/controller_images/controller_stick_right_press_0.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/controller_images/controller_stick_right_press_1.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/controller_images/controller_top.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/controller_images/spritesheet/A.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/controller_images/spritesheet/B.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/controller_images/spritesheet/Buttons.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/controller_images/spritesheet/X.png \
    build/Desktop_Qt_6_9_0_MinGW_64_bit-Debug/Images/controller_images/spritesheet/Y.png
