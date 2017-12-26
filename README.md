# mpi_mmulti.c

The program reads data from file and uses mpi to multiply matrices.

 ->The master sends Matrix A row by row to different slaves
 ->Slaves multipy the row received from master with the broadcasted Matrix B
 ->Slaves return the sum to the master
 ->The master gather results in the the resulting matrix

Instructions:

compile the code using: mpicc -o mpi_mmult mpi_mmult.c

ex: command line> mpirun -np #ofprocess <nameofyourprogram> data.txt rowA colA rowB ColB
rowA : number of rows in Matrix A
colA: number of columns in A
rowB : number of rows in Matrix B
colB: number of columns in B

Example:

1st matrix:
|1.00||2.00||0.00|
|1.00||1.00||0.00|
|1.00||1.00||1.00|

2nd matrix:
|2.00||1.00|
|1.00||2.00|
|1.00||1.00|

Result matrix:
|4.00||5.00|
|3.00||3.00|
|4.00||4.00|
