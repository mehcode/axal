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

    // auto menu_file = window.menuBar()->addMenu("&File");
    // QAction action_open("&Open...", &window);

    // menu_file->addAction(&action_open);

    // QObject::connect(&action_open, &QAction::triggered, []() {
    //     // TODO: Open ROM
    // });

    window.show();

    return app.exec();
}
