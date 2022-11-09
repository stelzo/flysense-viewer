# Flysense Viewer

Statemanager and GUI for FlySense. Give it images, it renders them with an overlay (defined by calls to an HTTP API) to a HDMI Stream.

```bash
sudo apt install libmicrohttpd-dev
git clone https://github.com/etr/libhttpserver.git
git checkout 0.18.2
cd libhttpserver
./bootstrap
mkdir build
cd build
../configure
make -j4
sudo make install
```

Tested on:

- TX2 Developer Kit
- AGX Xavier

```shell
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .

sudo make install
# sudo make uninstall
```
