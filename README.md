# Firefighter-problem-on-two-graph-classes
Algorithmic solution of the firefighter problem on cographs and split graphs.

## Compiling

The Makefile is suitable for Mac OSX with apple clang. For compiling on linux, remove the `-stdlib` flag.

```bash
make release # optimized release build
make debug   # includes debug symbols
```

## How to use

```
./ffp.a splitgraph <burning_idx> <graph_file> <delimiter>
./ffp.a splitgraph <burning_idx> <#clique> <#independent set>
./ffp.a cograph    <burning_idx> <graph_file1> <graph_file2>
```

### Examples

```bash
# Use sample splitgraph file as input
./ffp.a splitgraph 5 Graphen/Splitgraph_1.graphml 6

# Use two sample cograph files as input
./ffp.a cograph 3 Graphen/Cograph1_1.graphml Graphen/Cograph1_2.graphml
```
