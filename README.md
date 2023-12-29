# Explore the Mandelbrot Set

Code described in a [blog article][1] that allows you to

1. Explore the Mandelbrot set to find interesting parts of it
2. Generate images suitable for use as video conferencing backgrounds
3. Generate zooming videos and animated GIFs (See examples on [mandelbrot.dev][12])

## Gallery

### Videos

* [YouTube][4]
* [Tweet][5]
* [Tweet][6]

### Images

For more examples see several articles on [Yak Shavings][2], including exact coordinates for these images.

[![9]][9]
[![3]][3]
[![7]][7]
[![8]][8]
[![10]][10]
[![11]][11]

[1]: https://eamonn.org/programming/2021/01/02/almond-bread.html
[2]: https://eamonn.org
[3]: https://eamonn.org/img/mandelbrot_-0.19853719075520848_1.1001770019531256_0.00390625_100000.png
[4]: https://www.youtube.com/playlist?list=PLdUTefKASd0Zs68i6csV5rWCFnnANaJ6I
[5]: https://twitter.com/eob/status/1356120917533884419
[6]: https://twitter.com/eob/status/1356272749505699840
[7]: https://eamonn.org/img/mandelbrot_0.37001085813778134_0.6714354326948524_6e-8_100000.png
[8]: https://eamonn.org/img/mandelbrot_-0.748985544840495_0.05576807657877601_0.000244140625_100000.png
[9]: https://eamonn.org/img/mandelbrot_0_0_8_100000.png
[10]: https://eamonn.org/img/mandelbrot_0.250006_0_1.1920928955078125e-7_1000000.png
[11]: https://eamonn.org/img/mandelbrot_-1.292628079931202_-0.35266737725997915_7.275957614183426e-12_1000000.png
[12]: https://mandelbrot.dev

## Trying it yourself

### 1(a) - Launch from docker

(No need to clone this repo.)

```sh
docker run -p 3333:3333 eobrain/almondbread
```

or possibly you need `sudo`:

```sh
sudo docker run -p 3333:3333 eobrain/almondbread
```

### 1(b) - Or launch from this repo

(Only tested on Linux.)

* Prerequisites
  * Install Node.js
  * Install the g++ compiler for C++
  * To install dependencies locally, execute

    ```sh
    npm install
    ```

* To run a local web server
  * Execute

    ```sh
    npx bajel
    ```

### And then access by cisiring in browser

  * Visit http://localhost:3333/
  * Wait for initial image to load, then click on it to zoom
  * Right click to download an interesting image, or click the links above the
    image to download a zooming video or animated GIF.
