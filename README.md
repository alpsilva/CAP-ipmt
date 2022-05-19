# What is this tool?
Indexed pattern matching CLI tool (Also compression and decompression) for linux.

# How to compile
1. Enter "src" folder:
```
cd src
```

2. Compile the source code with make:
```
make
```

3. That's it!

# How to use

## Compression

ipmt can compress a text file:
```
ipmt zip some_text_file.txt 
```

ipmt can decompress the signature file extension ".myz":
```
ipmt unzip some_compressed_file.myz 
```

## Indexing

ipmt can index a text file:
```
ipmt index text_file.txt 
```

## exact match

ipmt can search in a index file:
```
ipmt [options] pattern file.idx
```

For a list of accepted options and commands, simply use:
```
ipmt -h 
```

If theres a need to delete the executable IPMT, simply use "make clean" to delete it.

```
make clean
```

# Authors
alps2 - André Luiz Pereira da Silva (alps2@cin.ufpe.br)

lvas - Lucas Vinícius Araújo da Silva (lvas@cin.ufpe.br)

# O que é essa ferramenta?
Ferramenta CLI para busca indexada de padrões (também capaz de comprimir e descomprimir arquivos de texto), para linux.

# Como compilar
1. Entre na pasta "src":
```
cd src
```

2. Compile o código fonte com make:
```
make
```

3. Feito!

# Como usar

## Compressão

ipmt é capaz de comprimir um arquivo de texto:
```
ipmt zip some_text_file.txt 
```

ipmt é capaz de descomprimir arquivos com a extensão própria ".myz":
```
ipmt unzip some_compressed_file.myz 
```

## indexação

ipmt consegue indexar um arquivo da forma seguinte:
```
ipmt index text_file.txt 
```

## busca exata

ipmt consegue buscar padrões em um arquivo indexado:
```
ipmt [options] pattern file.idx
```

Para uma lista de opções disponíveis, use:
```
ipmt -h 
```

Se existir necessidade de deletar o executável do IPMT, use "make clean".
```
make clean
```

# Autores
alps2 - André Luiz Pereira da Silva (alps2@cin.ufpe.br)

lvas - Lucas Vinícius Araújo da Silva (lvas@cin.ufpe.br)
