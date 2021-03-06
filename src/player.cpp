#include "player.h"
#include "viewport.h"

#include <QFileDialog>
#include <sstream>

ax::Player::Player(const char* filename) {
  setContentsMargins(1, 0, 1, 1);

  // Viewport (OpenGL Playfield)
  auto viewport = new Viewport(this);
  setCentralWidget(viewport);

  // Load core
  _runtime.core_load(viewport, filename);

  // Get info from core
  axal_info info;
  _runtime.get_info(&info);

  // Title
  std::stringstream title_stream;
  title_stream << "Axal - ";
  title_stream << info.library_name;
  title_stream << " (";
  title_stream << info.library_version;
  title_stream << ")";

  setWindowTitle(title_stream.str().c_str());

  // Size
  resize(128, 64);

  // Configure viewport for core
  viewport->setPixelFormat(info.pixel_format);

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

          // Show viewport
          this->viewport().setViewportVisible(true);

          // TODO: Centralize logic
          _action_reset->setEnabled(true);
          _action_close->setEnabled(true);
        }
      }
    }
  });

  _action_reset = new QAction("&Reset", this);
  _action_reset->setShortcut(Qt::CTRL + Qt::Key_R);
  _action_reset->setEnabled(false);

  connect(_action_reset, &QAction::triggered, this, [this] {
    if (_runtime.is_loaded() && _runtime.is_running()) {
      _runtime.pause();
      _runtime.reset();
      _runtime.resume();
    }
  });

  _action_close = new QAction("&Close", this);
  _action_close->setShortcut(Qt::CTRL + Qt::Key_W);
  _action_close->setEnabled(false);

  connect(_action_close, &QAction::triggered, this, [this] {
    if (_runtime.is_loaded() && _runtime.is_running()) {
      // Pause and remove ROM
      _runtime.pause();
      _runtime.rom_remove();

      // Hide viewport
      this->viewport().setViewportVisible(false);

      // TODO: Centralize logic
      _action_reset->setEnabled(false);
      _action_close->setEnabled(false);
    }
  });

  // Menu: File
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

void ax::Player::resize(unsigned w, unsigned h) {
  auto width = (w * 10) + 2;
  auto height = (h * 10) + 1;

  QMainWindow::resize(width, height);

  setMinimumWidth(width);
  setMaximumWidth(width);
  setMinimumHeight(height);
  setMaximumHeight(height);
}
