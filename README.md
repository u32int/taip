<h1 align="center">taip</h1>

A typing practice program written in C using SDL2. Visually inspired by projects like [monkeytype](https://github.com/monkeytypegame/monkeytype). It aims to be minimal on resources and unlike most modern typing programs be a desktop first application, usable without access to the internet.

<img title="" src="img/preview1.png" alt="">

<img title="" src="img/preview2.png" alt="">

### Building and running

**Dependencies**

cmake
sdl2
sdl2\_ttf
sdl2\_gfx

To build the project, use the following commands.

```bash
mkdir build && cd build && cmake .. && make
```

To run the project, execute the 'main' binary file

```bash
./main
```

### Disclaimer

This project is far from complete and currently in a "minimum viable product" state.

Expect big changes and more features as the development progresses.

### Contributing

All contributions are welcome. Using Github and submitting a pull request for smaller changes or creating an issue for larger ones is the prefered method.

### Licensing

This project is free software licensed under the MIT license. For the full text of the MIT license, read the 'LICENSE' file included with this project.

Liberation-Sans font included in the /fonts directory is licensed under the [OFL v1.1](https://github.com/liberationfonts/liberation-fonts/blob/main/LICENSE) license.

Some word lists have been adapted from the [monkeytype](https://github.com/monkeytypegame/monkeytype) codebase. These lists have been prefixed with 'mt_'.
