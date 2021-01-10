# Explore the Mandelbrot Set

* Prerequisites
  * Install Node.js
  * Install the g++ compiler for C++
  * To install dependencies locally, execute

    ```sh
    npm install
    ```

    * If you want to run tests

    ```sh
    sudo apt-get install libgtest-dev cmake
    cd /usr/src/gtest
    sudo cmake CMakeLists.txt
    sudo make
    sudo cp *.a /usr/lib
    ```

* To run a local web server
  * Execute

    ```sh
    npx bajel
    ```

  * Visit http://localhost:3000/
  