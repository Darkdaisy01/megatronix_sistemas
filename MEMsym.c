#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     // sleep

#define NUM_FILAS   8
#define TAM_LINEA   16
#define RAM_SIZE    4096

typedef struct {
    unsigned char ETQ;
    unsigned char Data[TAM_LINEA];
} T_CACHE_LINE;

// ---------- PROTOTIPOS OBLIGATORIOS ----------
void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]);
void VolcarCACHE(T_CACHE_LINE *tbl);
void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque);


int main(void) {
    T_CACHE_LINE cache[NUM_FILAS];
    unsigned char Simul_RAM[RAM_SIZE];

    int globaltime = 0;
    int numfallos  = 0;
    int accesos    = 0;

    LimpiarCACHE(cache);

    // ---------- CARGAR RAM ----------
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

    // ---------- ABRIR FICHERO DE DIRECCIONES ----------
    FILE *f_dir = fopen("dirs_memoria.txt", "r");
    if (!f_dir) {
        printf("ERROR: No se puede abrir dirs_memoria.txt\n");
        return -1;
    }

    // Para construir el texto leído
    char texto_leido[RAM_SIZE];
    int  pos_texto = 0;

    // ---------- BUCLE PRINCIPAL DE ACCESOS ----------
    unsigned int direccion;

    while (fscanf(f_dir, "%x", &direccion) == 1) {
        int palabra, linea, ETQ, bloque;
        ParsearDireccion(direccion, &ETQ, &palabra, &linea, &bloque);

        accesos++;
        globaltime++;        // cada acceso suma 1 unidad de tiempo

        if (cache[linea].ETQ == (unsigned char)ETQ) {
            // ---- ACIERTO ----
            unsigned char dato = cache[linea].Data[palabra];

            printf("T: %d, Acierto de caché, addr %04X etq %02X linea %02d dato %02X\n",
                   globaltime, direccion, ETQ, linea, dato);

            texto_leido[pos_texto++] = (char)dato;
        } else {
            // ---- FALLO ----
            numfallos++;

            printf("T: %d, Fallo de caché %d, addr %04X etq %02X linea %02d palabra %02X bloque %02X\n",
                   globaltime, numfallos, direccion, ETQ, linea, palabra, bloque);

            printf("Cargando el bloque %02X en la línea %02d\n",
                   bloque, linea);

            TratarFallo(cache, Simul_RAM, ETQ, linea, bloque);

            // Tras cargar, ya podemos leer el dato de caché
            unsigned char dato = cache[linea].Data[palabra];

            texto_leido[pos_texto++] = (char)dato;
        }

        // sleep de 1 segundo en cada acceso
        sleep(1);
    }

    fclose(f_dir);

    // ---------- VOLCADO FINAL DE CACHÉ POR PANTALLA ----------
    VolcarCACHE(cache);

    // ---------- ESTADÍSTICAS FINALES ----------
    double tiempo_medio = 0.0;
    if (accesos > 0)
        tiempo_medio = (double)globaltime / (double)accesos;

    printf("Accesos totales: %d; fallos: %d; Tiempo medio: %.2f\n",
           accesos, numfallos, tiempo_medio);

    texto_leido[pos_texto] = '\0';
    printf("Texto leído: %s\n", texto_leido);

    // ---------- VOLCADO A CONTENTS_CACHE.bin ----------
    FILE *f_cache = fopen("CONTENTS_CACHE.bin", "wb");
    if (f_cache) {
        // Byte 0 = byte 0 de la línea 0, etc.
        for (int i = 0; i < NUM_FILAS; i++) {
            fwrite(cache[i].Data, sizeof(unsigned char), TAM_LINEA, f_cache);
        }
        fclose(f_cache);
    } else {
        printf("ERROR: No se puede crear CONTENTS_CACHE.bin\n");
    }

    return 0;
}

void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]) {
    for (int i = 0; i < NUM_FILAS; i++) {
        tbl[i].ETQ = 0xFF;
        for (int j = 0; j < TAM_LINEA; j++) {
            tbl[i].Data[j] = 0x23;
        }
    }
}

// Vuelca la caché por pantalla en formato:
// ETQ  Datos: byte15 ... byte0
void VolcarCACHE(T_CACHE_LINE *tbl) {
    for (int i = 0; i < NUM_FILAS; i++) {
        printf("%02X    Datos:", tbl[i].ETQ);
        // de mayor a menor peso: 15 -> 0
        for (int j = TAM_LINEA - 1; j >= 0; j--) {
            printf("%02X ", tbl[i].Data[j]);
        }
        printf("\n");
    }
}

// Separa addr (12 bits) en ETQ(5), linea(3), palabra(4) y bloque
void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque) {
    *palabra = addr & 0xF;           // bits 0-3
    *linea   = (addr >> 4) & 0x7;    // bits 4-6
    *ETQ     = (addr >> 7) & 0x1F;   // bits 7-11
    *bloque  = addr / TAM_LINEA;     // cada bloque = 16 bytes
}

void TratarFallo(T_CACHE_LINE tbl[NUM_FILAS],unsigned char *Simul_RAM,int ETQ, int linea, int bloque)
{
    
}
