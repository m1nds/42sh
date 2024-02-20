# 42sh
Basic shell implementation in C that has features defined in the SCL such as :
- Basic shell scripting features (conditions, loops, etc...)
- Basic builtins (true, false, echo, dot, cd, pipelines, redirections, etc...)
- More advanced features like variables, subshells and functions. 
 
## Dependencies and building the project
These tools are required to build and run the kernel.
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
