## Ray Tracing Project

**Windows:**
```shell
$ cmake -B build
$ cmake --build build --config Release
$ build\Release\rayTracing.exe > image.ppm
```

**Mac:**
```shell
$ cmake -B build/Release -DCMAKE_BUILD_TYPE=Release
$ cmake --build build/Release
$ build/Release/rayTracing > image.ppm
```

## Acknowledgements & Credits

This project is a C++ implementation built by closely following the [*Ray Tracing in One Weekend*](https://raytracing.github.io/) book series by **Peter Shirley**, **Trevor David Black**, and **Steve Hollasch**.