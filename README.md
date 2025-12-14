# Hello and welcome to wbmps game of life,
which is basically the Conways game of life, but instead it works with WBMP images as frames. It is a small hobby project done by us two over the course of two weeks enjoy.

### Why C?
Mainly for the sake of challenge but also because you can easily do bitwise stuff in this language in my opinion.

### Why WBMP?
WBMP files are simple and only have values 0 = black and 1 = white. They are easy to deal with and I created the loader within a day, so...
They also don't take much that space, because you have only two colours. They are not compressed.

### Why SCons?
Godot uses it and it felt cool, so I said, why not -> We use it now. 
You want makefiles?\

*make* them yourself.

### Do we have future plans with this?
No.

#### Here is a list of things, that I will maybe implement sometime (ordered by Priority)
1. BMP input file, with either converting to greyscale or running the Game of Life once per RGB channel
2. The possibility to create a .sh/.bat file for simplicity when creating videos from the .wbmp frames.
3. A direct pipe to ffmpeg, so you can run this to get a video file.
4. Multithreading

### Should we add something?
If it is not a really, really good idea, feel free to make it yourself, because we won't do it. This is a small side-project and it isn't really a priority for us, so our focus lies elsewhere.

Thank you for your patience.
