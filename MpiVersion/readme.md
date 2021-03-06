

Fix the input and output paths in graphcoloring.c as per your requirement

```
make
mpirun -np 4 ./graphcoloring ../graph_file/anna.col   ../graph_file/anna.col.output

or
./run.sh // This will run all the .col files in ../graph_file/
```

output_filename contains the color assigned by Jones-Plassmann to each
vertex as well as the largest color assigned.

Notice that some of the files are in a binary format. They used the translation code
provided by cmu to translate those to the normal format.

For example DSJC1000.9.col.b was converted to DSJC1000.9.col and similarly
flat1000.76.col.b was converted to flat1000.76.col.

The input_filename is just the filename and is relative to my directory containing 
all these graph files. Change INPUT_PATH at the top of graphcoloring.c to point to
a different location. Similarly the OUTPUT_PATH constant at the top point to a
location somewhere in my account but it can be changed.

Jobs to compute strong scaling are in files called gcgraphfilenp.pbs. 
For example gcle450_5a8.pbs (8 cores). Jobs to compute weak scaling are in 
files called gcle450_5aorborcversusqueensnp.pbs.

One note about debugging: At the top of each of the .c files there are a 
series of directives that start with DEBUG_XYZ. If defined to be 1 they will
allow for debugging throughout the code. I set them to 0 after testing.

