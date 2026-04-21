ctetris
------

Simply Tetris in the terminal, based off the Tetris seen in the movie 'Tetris,'
i.e. just Tetris. How many times can this guy say Tetris?

# Main Use
I wanted to play around with making a project that can act as its own 'library'
type deal, so `ctetris` is a tetris backend, while the frontend is up to you
to create. I've got example frontends in the `example-frontends` directory
(shocker), which has a terminal and ncurses example.

# Installation
Just clone the repo to your local environment [it's not malware I swear ;)] and
use your build system for a C project. This is just standard library C and POSIX
functions, so I'll need to make it Windows compliant if you want to run it there.

Otherwise, if you're Mac/Linux, just running `make` from the project root will
build the binary `main` into the `bin` directory. Execute it from there or use
`make run` to do both.

## Frontend Examples
There are two examples in this project:
1) **terminal** (simple printing and ansi escape sequences)
2) **ncurses** (to be done still)

Change a precompilation macro in the Makefile, specifically `CDEFS` to change
which frontend example is created. `-DTEXTVIEW` is for the terminal only view
(Mac/Linux/Windows), while leaving an empty macro entirely will create the
ncurses view.

The terminal one's simple and just prints the backend state each iteration to the
screen, so as long as your terminal supports ANSI escape sequences, it'll look pretty.

https://github.com/user-attachments/assets/39ddcfd2-c47d-415f-9476-618fc77da3f7

I'm still working on the ncurses one, I wanted to do it as a mini-lesson in how to
use the library.

# TODO
- Add a oneliner compilation line for if you don't want to use `make`.
- Windows version of code.
- ncurses version of code.
- Confirm `ctetris` api looks clean/understandable.
- Clean up example frontend main file.
- Make option for each frontend.
- Make option for just the `ctetris` library (get rid of anything else/`.dll`s?).
