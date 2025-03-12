#include "planificatorui.h"
#include "retcodes/errorstack.h"
#include "config/configurationdata.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    qputenv("QT_QPA_PLATFORM","xcb");
    QApplication planificator(argc, argv);
    PlanificatorUi planificatorUi;
    rc::ErrorStack errors = config::data.initializeConfiguration();

    if(planificatorUi.init())
    {
        planificatorUi.setupUi();
        planificator.exec();
    }

    return 0;
}
