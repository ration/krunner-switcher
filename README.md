Switcher 
----------------------

Switcher is a KRunner addon, that allows fast-switching of windows by typing a dot and the application name or title, e.g. ".emacs"

Installing:
```
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=`kde4-config --prefix`
make
sudo make install
kquitapp krunner
krunner&
```