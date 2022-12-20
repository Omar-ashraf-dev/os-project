#include <stdio.h>
#include <stdlib.h>
#include <cs50.h>
#include <mpi.h>

// Functions prototype
void swap(int *xp, int *yp);
void selectionSort(int arr[], int n);

int main(int argc, char **argv)
{
  // [MPI variables] Process rank and size
  int rank, size;

  // [Prompt Variable] Array size
  const int arrSize = get_int("Enter the array size: ");

  // Initializing array with fixed size
  int localArray[arrSize];
  int sortedArray[arrSize];

  // This function initialize the MPI execution environment
  // It takes 2 parameters (pointer to the number of arguments, pointer to the argument vector)
  MPI_Init(&argc, &argv);

  // This function determines the rank of the calling process in the communicator
  // It takes 2 parameters (INPUT:communicator, OUTPUT:pointer to the rank of the calling process in the group of comm)
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // This function determines the size of the group associated with a communicator
  // It takes 2 parameters (INPUT:communicator, OUTPUT:pointer to the number of processes in the group of comm)
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // This function seeds the random number generator used by the function rand
  // seed − This is an integer value to be used as seed by the pseudo-random number generator algorithm
  srand(rank);

  // Fill the array with random numbers using rand() function
  for (int i = 0; i < arrSize; i++)
  {
    int var = rand() % 100;
    localArray[i] = var;
    sortedArray[i] = var;
  }

  // Display the random values for each array, the number of array based on size
  for (int proc = 0; proc < size; proc++)
  {
    if (rank == proc)
    {
      printf("Rank %i has values: [", rank);
      for (int i = 0; i < arrSize; i++)
        printf(" %d ", localArray[i]);
      printf("]\n");
    }

    // Blocks until all processes in the communicator have reached this routine
    // It takes only one argument (communicator)
    MPI_Barrier(MPI_COMM_WORLD);
  }

  // Sorting array
  selectionSort(sortedArray, arrSize);

  // Display sorted array
  printf("Rank %i sorted array: [", rank);
  for (int i = 0; i < arrSize; i++)
    printf(" %d ", sortedArray[i]);
  printf("]\n\n");

  // Using the sorted array to print the minimum value (index 0) and the maximum value (last index)
  printf("Using sorted array\n");
  // Display minimum value directly
  printf("Rank %d has lowest value of %d\n", rank, sortedArray[0]);

  // Display maximum value directly
  printf("Rank %d has maximum value of %d\n\n", rank, sortedArray[arrSize - 1]);

  // Find minimum value for each elements in array
  // Minimum variables to store the local and global variables
  int localMin[2];
  int globalMin[2];

  // Find minimum value in the array
  localMin[0] = localArray[0];
  for (int i = 1; i < arrSize; i++)
    if (localArray[i] < localMin[0])
      localMin[0] = localArray[i];
  localMin[1] = rank;

  // Reduce all of the local minimum value into the global minimum value in order to display minimum value for each array.
  // Combines values from all processes and distributes the result back to all processes
  // It takes 6 argument
  //           sendBuf: starting address of send buffer
  //           recvBuf: starting address of receive buffer
  //           count: number of elements in send buffer
  //           datatype: data type of elements of send buffer
  //           operation
  //           communicator
  MPI_Allreduce(localMin, globalMin, 1, MPI_2INT, MPI_MINLOC, MPI_COMM_WORLD);

  // Using the MPI to print the minimum value (MPI_MINLOC)
  printf("Using MPI\n");
  // Display the minimum value
  if (rank == 0)
  {
    printf("Rank %d has lowest value of %d\n", globalMin[1], globalMin[0]);
  }

  // Find minimum value for each elements in array
  // Minimum variables to store the local and global variables
  int localMax[2];
  int globalMax[2];

  // Find maximum value in the array
  localMax[0] = localArray[0];
  for (int i = 1; i < arrSize; i++)
    if (localArray[i] > localMax[0])
      localMax[0] = localArray[i];
  localMax[1] = rank;

  // Reduce all of the local maximum value into the global maximum value in order to display maximum value for each array.
  MPI_Allreduce(localMax, globalMax, 1, MPI_2INT, MPI_MAXLOC, MPI_COMM_WORLD);

  // Using the MPI to print the maximum value (MPI_MAXLOC)
  if (rank == 0)
  {
    printf("Rank %d has highest value of %d\n", globalMax[1], globalMax[0]);
  }

  // Terminates MPI execution environment
  MPI_Finalize();

  // End the program
  return 0;
}

// Function to swap two integers
void swap(int *xp, int *yp)
{
  int temp = *xp;
  *xp = *yp;
  *yp = temp;
}

// Function to perform Selection Sort
void selectionSort(int arr[], int n)
{
  int i, j, min_idx;

  // One by one move boundary of unsorted subArray
  for (i = 0; i < n - 1; i++)
  {

    // Find the minimum element in unsorted array
    min_idx = i;
    for (j = i + 1; j < n; j++)
      if (arr[j] < arr[min_idx])
        min_idx = j;

    // Swap the found minimum element
    // with the first element
    swap(&arr[min_idx], &arr[i]);
  }
}