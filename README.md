# uv_secnet

Secure networking library built upon OpenSSL, LibUV and http-parser

## Compiling steps

1. Create a build directory:

    ```
    $ mkdir build && cd build
    ```

2. Install dependencies:

    ```
    conan install ..
    ```

3. Configure the CMake project:

    ```
    cmake ..
    ```

4. Build it:

    ```
    make -j <num_cpu>
    ```

5. Install the library:

    ```
    make install
    ```
