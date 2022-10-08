# Portfolio

The homepage of my [online portfolio](https://arielmalka.com).

### Instructions for building WASM + WebGL version on macOS or Linux:

1. Install Emscripten and build the chronotext-cross library following [these instructions](https://arielm.github.io/cross-blog/2022/10/06/running-in-the-browser.html)

2. Clone this repository with:
   ```
   git clone https://github.com/arielm/works
   ```

3. Add `WORKS_PATH` variable:
   ```
   export WORKS_PATH=path/to/works
   ```

4. Build and run:
   ```
   cd $WORKS_PATH/sketches/Portfolio
   RUN_TEST -DPLATFORM=emscripten
   ```
   
   The resulting `.wasm`, `.js` and `.data` files will be in `build/emscripten`.
