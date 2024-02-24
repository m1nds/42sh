# 42sh
Basic shell implementation in C that has features defined in the SCL such as :
- Basic shell scripting features (conditions, loops, etc...)
- Basic builtins (true, false, echo, dot, cd, pipelines, redirections, etc...)
- More advanced features like variables, subshells and functions. 

The project has been made with a team of 4 people. We used GitHub features
such as pull requests and continuous integration for a better workflow.

![CI Status](https://github.com/m1nds/42sh/actions/workflows/ci.yml/badge.svg)

## Dependencies and building the project
These tools are required to build the shell.
- Autotools
- Make
- A C compiler

```bash
[m1nds@arch 42sh]$ autoreconf --install && ./configure
```

```bash
[m1nds@arch 42sh]$ make
```


## How to run the testsuite ?

After issuing :
```bash
[m1nds@arch 42sh]$ autoreconf --install && ./configure
```

You just have to make this command : 
```bash
[m1nds@arch 42sh]$ make check
```
