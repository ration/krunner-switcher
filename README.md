Switcher
----------------------

Switcher is a KRunner addon, that allows fast-switching of windows by typing a dot and the application name or title, e.g. ".emacs". To use it effectively make sure this plugin is top priority in Plasma settings.

### Required Dependencies

Debian/Ubuntu:
`sudo apt install cmake extra-cmake-modules build-essential libkf6runner-dev libkf6i18n-dev qt6-base-dev`

openSUSE:
`sudo zypper install cmake extra-cmake-modules libQt6Core6 kf6-ki18n-devel kf6-krunner-devel gettext-tools`

Fedora:
`sudo dnf install cmake extra-cmake-modules kf6-ki18n-devel kf6-krunner-devel gettext`

### Installing:
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
sudo make install
kquitapp6 krunner 2> /dev/null; kstart krunner > /dev/null 2>&1 &
```

