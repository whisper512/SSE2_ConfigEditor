#include "SSE2_ConfigEditorMainWnd.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    SSE2_ConfigEditorMainWnd window;
    window.show();
    return app.exec();
}
