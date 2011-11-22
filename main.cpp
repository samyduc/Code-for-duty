/*
    Coding Challenge : Code for duty

    @description: Flatten a vector
    @comment: Works on Windows compiled with GCC
    @author: Samy Duc nobunaga.duc@gmail.com
 */

#include <stdio.h>
#include <stdlib.h>

#define FILE_INPUT "input.txt"
#define FILE_INPUT_RIGHT "r"
#define FILE_OUTPUT "output.txt"
#define FILE_OUTPUT_RIGHT "w"
#define FILE_EOF 0

#define FLATTEN_KO -1

typedef unsigned int u_int;

/****************************************
        Utils function
 ***************************************/

void /* print a vector to screen for debugging*/
print_vector(u_int* vector, u_int vector_size)
{
    u_int i;

    printf("\n(");

    for(i=0; i<vector_size; ++i)
    {
        printf("%d", vector[i]);
        if(i+1 != vector_size)
        {
             printf(",");
        }
    }

    printf(")\n");
}

void /* print iteration number at the beginning of report */
print_vector_iteration(FILE *file_descriptor, u_int nb_iteration)
{
    static long file_position;
    // small hack to detect the iteration number (because we do not know this number before iterations)
    if(nb_iteration == 0)
    {
        // save current position
        file_position = ftell(file_descriptor);
        fprintf(file_descriptor, "\n\n");
    }
    else
    {
        //go back to previous position to write iteration number
        fseek(file_descriptor, file_position, SEEK_SET);
        fprintf(file_descriptor, "%d\n", nb_iteration);
        fseek(file_descriptor, 0, SEEK_END);
        fprintf(file_descriptor, "\n");
    }
}

void /* print a vector to output file */
print_vector_screen(FILE *file_descriptor, u_int* vector, u_int vector_size, u_int nb_iteration)
{
    if(vector_size == FLATTEN_KO)
    {
        // not flattenable
        fprintf(file_descriptor, "%d\n\n", FLATTEN_KO);
    }
    else
    {
        u_int i;

        fprintf(file_descriptor, "%d : (", nb_iteration);

        for(i=0; i<vector_size; ++i)
        {
            fprintf(file_descriptor, "%d", vector[i]);
            if(i+1 != vector_size)
            {
                 fprintf(file_descriptor, ",");
            }
        }

        fprintf(file_descriptor, ")\n");
    }
}

u_int* /* get next vector in input file with its size*/
getNextVector(FILE *file_descriptor, u_int *vector_size)
{
    u_int* vector = NULL;

    // read size
    fscanf(file_descriptor, "%d", vector_size);

    // read value
    if(*vector_size != FILE_EOF)
    {
        u_int i;

        // allocate vector and fill it with input file
        vector = (u_int*)malloc(sizeof(u_int) * (*vector_size));
        for(i=0; i<*vector_size; ++i)
        {
           fscanf(file_descriptor, "%d", &(vector[i]));
        }
    }

    return vector;
}

/****************************************
        Algorithm function
 ***************************************/

int /* try to flatten a vector returning */
check_flatten(u_int* vector, u_int vector_size)
{
    // vector's values must take that value if possible
    u_int i;
    int flat_number = vector[0];

    // do the average
    for(i=1; i<vector_size; ++i)
    {
        flat_number += vector[i];
    }

    // if average is not an integer
    if(flat_number % vector_size != 0)
    {
        flat_number = FLATTEN_KO;
    }
    else
    {
        flat_number /= vector_size;
    }

    return flat_number;
}

u_int /* iterate through a vector and return the number of operation*/
iterate_vector(u_int* vector, u_int vector_size, u_int flat_number)
{
    u_int i;
    u_int ok = 0;

    // new array to store movement
    u_int* array_change = (u_int*)malloc(sizeof(u_int)*vector_size);

    // iterate left to right
    for(i=0; i<vector_size-1; ++i)
    {
        if(vector[i] > flat_number)
        {
            // if next is not the end of loop
            if(i + 1 == vector_size - 1 && vector[vector_size - 1] == flat_number)
            {
                array_change[i] = 0;
                break;
            }
            // do push to it (avoid infinite loop)
            vector[i] --;
            vector[i+1]++;
            array_change[i] = 1;
        }
        else
        {
            array_change[i] = 0;
        }
    }
    array_change[vector_size-1] = 0;

    // iterate right to left
    for(i=vector_size-1; i>0; --i)
    {
        if(array_change[i] == 0 && vector[i] > flat_number)
        {
            vector[i] --;
            vector[i-1]++;
            array_change[i] = 1;
        }
        else
        {
            array_change[i] = 0;
        }
        // check in the loopback if it is flatten (avoid an extra loop)
        if(vector[i] != flat_number)
        {
            ok++;
        }
    }

    free(array_change);

    return ok;
}

void /* try to flatten a vector */
flatten_vector(u_int* vector, u_int vector_size, FILE *file_output)
{
    // check if possible to flatten
    u_int flat_number;

    flat_number = check_flatten(vector, vector_size);

    // do not try to flatten
    if(flat_number == FLATTEN_KO)
    {
        print_vector_screen(file_output, vector, FLATTEN_KO, 0);
    }
    else
    {
        u_int nb_iteration = 0;
        print_vector_iteration(file_output, nb_iteration);
        do
        {
            print_vector_screen(file_output, vector, vector_size, nb_iteration);
            ++nb_iteration;
        }
        while(iterate_vector(vector, vector_size, flat_number) != 0);

        // print last result
        print_vector_screen(file_output, vector, vector_size, nb_iteration);
        print_vector_iteration(file_output, nb_iteration);
    }
}


/****************************************
        Entry point
 ***************************************/

int /* process a file and write ouptut */
main(void)
{
    FILE *file_input = fopen(FILE_INPUT, FILE_INPUT_RIGHT);
    FILE *file_output = fopen(FILE_OUTPUT, FILE_OUTPUT_RIGHT);
    u_int* vector = NULL;
    u_int vector_size = 0;

    if(file_input)
    {

        // infinite loop !
        while(1)
        {
            vector = getNextVector(file_input, &vector_size);

            // check if no EOF
            if(vector != NULL)
            {
                // do flatten algorithm
                print_vector(vector, vector_size);
                flatten_vector(vector, vector_size, file_output);
                free(vector);
            }
            else
            {
                // end
                fclose(file_input);
                fclose(file_output);
                break;
            }
        }

    }

    return 0;
}
