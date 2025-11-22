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

    //Usar dos bucles para poner a 0xFF (hexadecimal) el campo etiqueta (ETQ) y a 0x23 el campo de datos de cada línea de la caché.
    for (int i = 0; i < NUMFILAS; i++) {
        cache[i].ETQ = 0xFF;
        for (int j = 0; j < TAM_LINEA; j++) {
            cache[i].Data[j] = 0x23;
        }
    }

    //Cargar el archivo binario CONTENTS_RAM.bin en SimulRAM
    FILE *file_ram = fopen("CONTENTS_RAM.bin", "rb");
    if (file_ram == NULL) {
        printf("ERROR: No se puede abrir CONTENTS_RAM.bin\n");
        return -1;
    }
    size_t leidos = fread(SimulRAM, sizeof(unsigned char), RAM_SIZE, file_ram);
    if(leidos != RAM_SIZE) {
        printf("ERROR: No se pudo leer la RAM completa\n");
        fclose(file_ram);
        return -1;
    }
    fclose(file_ram);



    FILE *file_adress = fopen("addresses.txt", "r");
    if (file_adress == NULL){
        printf("ERROR: no se ha podido abrir el archivo addresses.txt\n");
        return -1;
    }

    return 0;
}
