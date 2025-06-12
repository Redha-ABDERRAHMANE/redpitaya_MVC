#include "view.h"
#include <QtWidgets/QApplication>
#include "MVC_Controller.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    View window;
    MVC_Controller mvc_controller(&window);




    window.show();
    return app.exec();
}
