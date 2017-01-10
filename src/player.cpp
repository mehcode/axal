#include "player.h"
#include "viewport.h"

#include <QFileDialog>

ax::Player::Player(const char* filename) {
  setContentsMargins(1, 0, 1, 1);
  setWindowTitle("Axal");

  // Size
  // TODO: Get size from last run
  auto width = (64 * 10) + 2;
  auto height = (32 * 10) + 1;
  resize(width, height);
  setMinimumWidth(width);
  setMaximumWidth(width);
  setMinimumHeight(height);
  setMaximumHeight(height);

  // Viewport (OpenGL Playfield)
  auto viewport = new Viewport(this);
  setCentralWidget(viewport);

  // Load core
  _runtime.core_load(viewport, filename);

  // Setup video_refresh
  _runtime.set_video_refresh(&Viewport::_video_refresh);

  // Actions/Commands
  _action_open = new QAction("&Open...", this);
  _action_open->setShortcut(Qt::CTRL + Qt::Key_O);

  connect(_action_open, &QAction::triggered, this, [this] {
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::Detail);

    if (dialog.exec()) {
      auto filenames = dialog.selectedFiles();
      if (filenames.size() > 0) {
        auto filename = filenames.at(0);

        if (_runtime.is_loaded()) {
          _runtime.pause();
          _runtime.rom_insert(filename.toUtf8().constData());
          _runtime.reset();
          _runtime.resume();
        }
      }
    }
  });

  _action_reset = new QAction("&Reset", this);
  _action_reset->setShortcut(Qt::CTRL + Qt::Key_R);
  _action_reset->setEnabled(false);

  _action_close = new QAction("&Close", this);
  _action_close->setShortcut(Qt::CTRL + Qt::Key_W);
  _action_close->setEnabled(false);

  // Menu
  auto menu_file = menuBar()->addMenu("&File");
  menu_file->addAction(_action_open);
  menu_file->addSeparator();
  menu_file->addAction(_action_reset);
  menu_file->addSeparator();
  menu_file->addAction(_action_close);
}

ax::Player::~Player() noexcept {
}

auto ax::Player::viewport() noexcept -> Viewport& {
  return *((Viewport*)centralWidget());
}
