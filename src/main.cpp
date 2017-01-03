#include <cstdio>

#include <QApplication>
// #include <QMainWindow>
// #include <QWidget>
// #include <QMenu>
// #include <QMenuBar>
// #include <QMessageBox>

#include "window.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    axal::Window window;

    window.show();

    return app.exec();
}
