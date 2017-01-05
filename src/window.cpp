#include "window.h"
#include "viewport.h"

#include <QFileDialog>

ax::Window::Window() : _open(NULL), _close(NULL) {
  // Layout
  setContentsMargins(1, 1, 1, 1);
  setWindowTitle("Axal");

  // Size
  auto width = (64 * 10) + 1;
  auto height = (32 * 10) + 1;
  resize(width, height);
  setMinimumWidth(width);
  setMaximumWidth(width);
  setMinimumHeight(height);
  setMaximumHeight(height);

  // Viewport
  setCentralWidget(new Viewport(this));

  // Actions
  _action_open = new QAction("&Open...", this);

  connect(_action_open, &QAction::triggered, this, [this] {
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::Detail);

    if (dialog.exec()) {
      auto filenames = dialog.selectedFiles();
      if (filenames.size() > 0) {
        auto filename = filenames.at(0);

        if (_open) {
          _open(filename.toStdString().c_str());
        }
      }
    }
  });

  _action_close = new QAction("&Close", this);
  _action_close->setEnabled(false);

  connect(_action_close, &QAction::triggered, this, [this] {
    // [...]
  });

  // Menu
  auto menu_file = menuBar()->addMenu("&File");
  menu_file->addAction(_action_open);
  menu_file->addAction(_action_close);
}

ax::Window::~Window() noexcept {
}

auto ax::Window::viewport() noexcept -> Viewport& {
  return *((Viewport*)centralWidget());
}
