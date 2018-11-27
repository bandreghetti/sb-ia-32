# Assembly UnB -> Assembly IA-32

To compile the translator, run:

```bash
$ ./compileProject
```

After compiling the translator, you can test it with any file you prefer. Some test files are provided.

```bash
$ ./tradutor.out test-files/triangulo
```

The translator will output the `test-files/triangulo.s` file. To assemble it with Netwide Assembler (nasm) and link it with GNU Linker (ld):

```bash
$ ./assembleAndLink.sh test-files/triangulo.s
```

The script will output the `test-files/triangulo.e` which is executable.
