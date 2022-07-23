Switcher 
----------------------

Switcher is a KRunner addon, that allows fast-switching of windows by typing a dot and the application name or title, e.g. ".emacs"

### Required Dependencies

Debian/Ubuntu:  
`sudo apt install cmake extra-cmake-modules build-essential libkf5runner-dev libkf5textwidgets-dev qtdeclarative5-dev gettext libqt5x11extras5-dev libxcb-randr0-dev`

openSUSE:  
`sudo zypper install cmake extra-cmake-modules libQt5Widgets5 libQt5Core5 libqt5-qtlocation-devel ki18n-devel
ktextwidgets-devel kservice-devel krunner-devel gettext-tools`  

Fedora:  
`sudo dnf install cmake extra-cmake-modules kf5-ki18n-devel kf5-kservice-devel kf5-krunner-devel kf5-ktextwidgets-devel gettext`  

### Installing:
```
mkdir build
cd build
cmake -DQT_PLUGIN_INSTALL_DIR=`kf5-config --qt-plugins` -DCMAKE_BUILD_TYPE=Release  ..
make
sudo make install
kquitapp5 krunner 2> /dev/null; kstart5 --windowclass krunner krunner > /dev/null 2>&1 &
```

#### Screenshot of overview
![Screenshot of overview](https://raw.githubusercontent.com/alex1701c/Screenshots/master/krunner-switcher/overview.png)
