# expressvpn-applet

Unofficial Linux applet for ExpressVPN.

![expressvpn-applet.png](screenshot.png "expressvpn-applet.png")

## Compilation

Dependencies:

- boost
- gtkmm-3.0

```
mkdir build
cmake . -B build -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_BUILD_TYPE=Release
cmake --build build
cmake --install build
```

## License

GNU GPL-3

## Contribution

Feel free to fork and create pull request.