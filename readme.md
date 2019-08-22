# rsh

## Build:

rsh uses the [meson](https://mesonbuild.com/) build system. To install, either `apt install meson` or `brew install meson`.

Then:

```
meson build
ninja -C build
```

The executable can be found at `bin/rsh`.


### Components:

* `builtin.c`: matches the input against the list of builtins; currently `cd`, `pwd`, `echo`, `ls`, `pinfo`, `nightswatch`, `dirty`, `interrupt`, `history` and `exit`
* `history.c`: responsible for loading the history list from disk, maintaining it in memory, and persisting to file on exit
* `ls.c`: implements the `ls` builtin. The `-l` and `-a` options are implemented
* `nightswatch.c`: implements the `nightswatch` builtin; supports the `dirty` and `interrupts` sub-commands
* `pinfo.c`: implements the `pinfo` builtin to fetch process information. Not supported on macOS due to lack of a procfs
* `external.c`: runs external commands (non-builtins); responsible for putting newly created processes into their own groups and handling terminal control
* `interpret.c`: checks if the input matches a builtin, otherwise delegates to the external handler
* `main.c`: runs the main REPL loop. The main shell state data structure is also defined in `main.h`. Also responsible for initializing the shell; handling `SIGCHLD`, and saving the existing term attributes for restoring later
* `parse.c`: tokenizes and parses the input line. Supports semicolon delimited commands, optionally ampersand terminated to indicate a background job
* `prompt.c`: generates the prompt
