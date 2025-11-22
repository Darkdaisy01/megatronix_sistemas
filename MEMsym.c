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
    T_CACHE_LINE cache[NUMFILAS];
    unsigned char Simul_RAM[RAM_SIZE];
    int globaltime = 0, numfallos = 0;

    // Inicialización de la caché
    LimpiarCACHE(cache);

    // --- Abrir y cargar RAM binaria ---
    FILE *f_ram = fopen("CONTENTS_RAM.bin", "rb");
    if (!f_ram) {
        printf("ERROR: No se puede abrir CONTENTS_RAM.bin\n");
        return -1;
    }
    size_t leidos = fread(Simul_RAM, sizeof(unsigned char), RAM_SIZE, f_ram);
    if (leidos != RAM_SIZE) {
        printf("ERROR: No se pudo leer la RAM completa\n");
        fclose(f_ram);
        return -1;
    }
    fclose(f_ram);

    // --- Abrir fichero de direcciones ---
    FILE *f_dir = fopen("dirs_memoria.txt", "r");
    if (!f_dir) {
        printf("ERROR: No se puede abrir dirs_memoria.txt\n");
        return -1;
    }

    // --- Procesamiento principal ---
    unsigned int direccion;
    while (fscanf(f_dir, "%x", &direccion) == 1) {
        int palabra, linea, ETQ, bloque;
        ParsearDireccion(direccion, &ETQ, &palabra, &linea, &bloque);

        if (cache[linea].ETQ == ETQ) {
            // Acierto de caché
            printf("ACIERTO - Dirección %03X en línea %d, etiqueta %02X\n", direccion, linea, ETQ);
            // Aquí puedes mostrar cache[linea].Data[palabra]
        } else {
            // Fallo de caché
            printf("FALLO  - Dirección %03X en línea %d, etiqueta cache %02X, etiqueta addr %02X\n",
                direccion, linea, cache[linea].ETQ, ETQ);
            TratarFallo(cache, Simul_RAM, ETQ, linea, bloque);
            numfallos++;
        }
        globaltime++; // Incrementa tiempo, si así lo pide el enunciado
    }

    fclose(f_dir);

    // --- Volcado final de la caché ---
    FILE *fout = fopen("CONTENTS_CACHE.bin", "wb");
    if (fout) {
        fwrite(cache, sizeof(T_CACHE_LINE), NUMFILAS, fout);
        fclose(fout);
    }

    // --- Mensaje final de estadísticas ---
    printf("Total de fallos de caché: %d\n", numfallos);

    return 0;
}

void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque) {

    *palabra = addr & 0xF;           // Bits 0-3
    *linea   = (addr >> 4) & 0x7;    // Bits 4-6
    *ETQ     = (addr >> 7) & 0x1F;   // Bits 7-11
    // *bloque se puede calcular como addr / 16
    *bloque  = addr / 16;
}


void TratarFallo(T_CACHE_LINE tbl[NUMFILAS], unsigned char *Simul_RAM, int ETQ, int linea, int bloque) {
    tbl[linea].ETQ = ETQ;
    // Cada línea de caché contiene 16 bytes. El bloque inicia en RAM en bloque*16.
    int inicio = bloque * 16;
    for (int i = 0; i < 16; i++) {
        tbl[linea].Data[i] = Simul_RAM[inicio + i];
    }
}

