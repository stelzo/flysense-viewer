# Flysense Viewer

Statemanager and GUI for FlySense. Give it images, it renders them with an overlay (defined by calls to an HTTP API) to a HDMI Stream.

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
