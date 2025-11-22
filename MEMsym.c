#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 

#define NUMFILAS 8
#define TAM_LINEA 16
#define RAM_SIZE 4096


typedef struct {
unsigned char ETQ;
unsigned char Data[TAM_LINEA];
} T_CACHE_LINE;

void LimpiarCACHE(T_CACHE_LINE tbl[NUMFILAS]);
void VolcarCACHE(T_CACHE_LINE tbl[NUMFILAS]);
void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque);
void TratarFallo(T_CACHE_LINE tbl[NUMFILAS], unsigned char *MRAM, int ETQ, int linea, int bloque);


int main() {
    
    int globaltime = 0;
    int numfallos = 0;
    T_CACHE_LINE cache[NUMFILAS];
    unsigned char SimulRAM[RAM_SIZE];

    printf("BOOOOMM\n");

    return 0;
}
