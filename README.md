# Gazō image processor
This project is only me exploring/learning C++. Not intended for serious use. But feedback is welcome tho 😀
## Dependencies
- Crow
- Magick++

On Arch Linux, you can use:
```
yay -S crow imagemagick
```

## Building
Create a new folder called `build` and then
```
cmake -B build .
make -C build
```
The executable called `gazou` will be available inside the `build` directory.
