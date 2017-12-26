/*


The program reads data from file and uses mpi to multiply matrices.
The master sends Matrix A row by row to different slaves
Slaves multipy the row received from master with the broadcasted Matrix B
Slaves return the sum to the master
The master gather results in the the resulting matrix

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


*/

#include<stdio.h>
#include<mpi.h>
#include <stdlib.h>
#define min(x, y) ((x)<(y)?(x):(y))



int rank; //process rank
int size; //number of processes
int i, j, k, row,anstype, sender; //helper variables
int NUM_ROWS_A;  //rows of input [A]
int NUM_COLUMNS_A;//columns of input [A]
int NUM_ROWS_B;   //rows of input [B]
int NUM_COLUMNS_B; //columns of input [B]
int numsent;
double start_time; //hold start time
double end_time; // hold end time
int low_bound; //low bound of the number of rows of [A] allocated to a slave
int upper_bound; //upper bound of the number of rows of [A] allocated to a slave
int portion; //portion of the number of rows of [A] allocated to a slave
MPI_Status status; // store status of a MPI_Recv
MPI_Request request; //capture request of a MPI_Isend
double *mat_a, *mat_b, *mat_result, *buffer, *ans;




int main(int argc, char *argv[])
{

           int i;
   
   if(argc == 6){
   
   
          NUM_ROWS_A = atoi(argv[2]);
          NUM_COLUMNS_A = atoi(argv[3]);
          NUM_ROWS_B = atoi(argv[4]);
          NUM_COLUMNS_B = atoi(argv[5]);
                  
		  mat_b = (double*)malloc(sizeof(double) * NUM_COLUMNS_B * NUM_ROWS_B);
		  mat_a = (double*)malloc(sizeof(double) * NUM_COLUMNS_A * NUM_ROWS_A);
		  mat_result = (double*)malloc(sizeof(double) * NUM_COLUMNS_B * NUM_ROWS_A);
		  buffer = (double*)malloc(sizeof(double) * NUM_COLUMNS_A);
		  ans = (double*)malloc(sizeof(double) * NUM_COLUMNS_A);
			  

    MPI_Init(&argc, &argv); //initialize MPI operations
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); //get the rank
    MPI_Comm_size(MPI_COMM_WORLD, &size); //get number of processes

   
    
    if (size < 2)
    {
        printf("Please run with two processes.\n");fflush(stdout);
        MPI_Finalize();
        return 0;
    }
    
    
     

    /* master initializes work*/
    if (rank == 0) {
    
            FILE *fptr; 
 
      fptr = fopen(argv[1],"r");
		  if (fptr == NULL)
			{
				perror(argv[1]);
			return EXIT_FAILURE;
			}
     

     
		
             double temp;
			  
			  for(i = 0; i < NUM_ROWS_A; i++){
			   for(j = 0; j < NUM_COLUMNS_A; j++){
				  fscanf(fptr, "%lf", &temp);
				  mat_a[i * NUM_COLUMNS_A + j] = temp;
				}  
			 }
			 
			
     
     
      
			  //fill the 2nd matrix up with data from file
     for(i = 0; i < NUM_ROWS_B; i++){
       for(j = 0; j < NUM_COLUMNS_B; j++){
          fscanf(fptr, "%lf", &temp);
          mat_b[i * NUM_COLUMNS_B + j] = temp;
       }   
     }
     
     
     
     fclose(fptr);
     
     
			  
			   if (NUM_COLUMNS_A != NUM_ROWS_B){
            perror("Matrices with entered orders can't be multiplied with each other:");
              MPI_Abort(MPI_COMM_WORLD,0);
     }
			  
			  

   
        //broadcast [B] to all the slaves
    MPI_Bcast(mat_b, NUM_ROWS_B*NUM_COLUMNS_B, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        
        numsent = 0;
        int count = 0;
           
         //Storing A row by row in buffer and send the to slave process
                for (i = 0; i < min(size-1, NUM_ROWS_A); i++) {
                    for (j = 0; j < NUM_COLUMNS_A; j++) {
                        buffer[j] = mat_a[i * NUM_COLUMNS_A + j];
                        
                        
                    }
                     
                   		
                      
                    MPI_Send(buffer, NUM_COLUMNS_A, MPI_DOUBLE, i+1, i+1, MPI_COMM_WORLD);
                    numsent++;
                }
               
                
                 
                /* master gathers processed work*/
                     
                    for (k = 0; k < NUM_ROWS_A; k++) {
                       
                    MPI_Recv(ans, NUM_COLUMNS_B, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG,
                             MPI_COMM_WORLD, &status);
                           
                    sender = status.MPI_SOURCE;
                    anstype = status.MPI_TAG;
                            
                    
                    for(i = 0; i < NUM_COLUMNS_B; i++){
                       mat_result[count] = ans[i];
                       
                       count++;
                    }
                     
 					
                    
                    
                    //if the number of rows greater than processes
                    if (numsent < NUM_ROWS_A) {
                       for (j = 0; j < NUM_COLUMNS_A; j++) {
                            buffer[j] = mat_a[numsent * NUM_COLUMNS_A + j];
                            
                        }
                           
                           
                        MPI_Send(buffer, NUM_COLUMNS_A, MPI_DOUBLE, sender, numsent+1,
                                 MPI_COMM_WORLD);
                                 
                              
                        numsent++;
                        
                    } else {
                        
                        MPI_Send(MPI_BOTTOM, 0, MPI_DOUBLE, sender, 0, MPI_COMM_WORLD);
                    }
                    
                     
                     
                    
                }
                
                
            //  fflush(stdout); 
            printf("1st matrix:\n");
             for(i = 0; i < NUM_ROWS_A; i++){
       for(j = 0; j < NUM_COLUMNS_A; j++)
          printf("|%.2f|", mat_a[i * NUM_COLUMNS_A + j]);
       printf("\n");   
     }
            
            printf("2nd matrix:\n");
          for(i = 0; i < NUM_ROWS_B; i++){
       for(j = 0; j < NUM_COLUMNS_B; j++)
          printf("|%.2f|", mat_b[i * NUM_COLUMNS_B + j]);
       printf("\n");   
     } 
               
        printf("\n\nResult matrix:\n");
        for (i = 0; i < NUM_ROWS_A; i++) {
        for (j = 0; j < NUM_COLUMNS_B; j++){
            printf("|%.2f|", mat_result[i * NUM_COLUMNS_B +j]);
            }
         printf("\n");    
    }
        
         end_time = MPI_Wtime();
        printf("\nRunning Time = %f\n\n", end_time - start_time);
       
        
       
				
    } else {
    
     
    
    /* work done by slaves*/
   
   
       // Slave Code goes here
             MPI_Bcast(mat_b, NUM_ROWS_B*NUM_COLUMNS_B, MPI_DOUBLE, 0, MPI_COMM_WORLD);
             
						while(1){
                
                MPI_Recv(buffer, NUM_COLUMNS_A, MPI_DOUBLE, 0, MPI_ANY_TAG,
                                  MPI_COMM_WORLD, &status);
                                     
                         // printf("\n");       
                        
							
                        if (status.MPI_TAG == 0){
                             
                            break;
                        }
                        
                        double sum;
                        row = status.MPI_TAG;
                        
                        
                        
                        printf("\n");
                        for(i = 0; i < NUM_COLUMNS_B; i++){
                        		sum = 0.0;
                           for (j = 0; j < NUM_COLUMNS_A; j++) {
                                    
									sum = sum + buffer[j] * mat_b[j * NUM_COLUMNS_B + i];
									
									
                        }
						
                         ans[i] = sum;
                         
                         
                        }
                            
                            
                            MPI_Send(ans, NUM_COLUMNS_B, MPI_DOUBLE, 0, row, MPI_COMM_WORLD);
                                  
                        
                    
                 }   
                  
                
            }			
    
    
    
    
    } else{
     fprintf(stderr, "Usage mpi_mmult <filename> rowA colA rowB ColB\n");
      return 0;
     }
     
					  
					free(mat_b);
					free(mat_a);
					free(buffer);
					free(ans);
					free(mat_result);
     
     
    
    MPI_Finalize(); //finalize MPI operations
    return 0;
}



