#include "transformdll.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>


// Variables estáticas para almacenar las coordenadas de posición
static float posX = 0.0f;
static float posY = 0.0f;
static float posZ = 0.0f;


extern "C" {

// Función para establecer la posición inicial en todos los ejes
__attribute__((visibility("default")))  void SetTransformPosition(float x, float y, float z) {
    posX = x;
    posY = y;
    posZ = z;

    FILE *logFile = fopen("/home/daniel/Escritorio/transform_log.txt", "a");
    if (logFile) {
        fprintf(logFile, "posX: %f, posY: %f, posZ: %f\n", posX, posY, posZ);
        fclose(logFile);
    }
}

// Función para obtener la posición X actual
__attribute__((visibility("default"))) float GetPositionX() {
    FILE *file = fopen("/home/daniel/Escritorio/transform_log.txt", "r");
    if (file == NULL) {
        perror("No se pudo abrir el archivo");
        return posX;
    }

    char line[256];
    char lastLine[256];

    while (fgets(line, sizeof(line), file)) {
        strncpy(lastLine, line, sizeof(lastLine) - 1);
        lastLine[sizeof(lastLine) - 1] = '\0';
    }
    float posX, posY, posZ;
    int numItems = sscanf(lastLine, "posX: %f, posY: %f, posZ: %f", &posX, &posY, &posZ);

    if (numItems == 3) {
        printf("PosX: %f, PosY: %f, PosZ: %f\n", posX, posY, posZ);
    } else {
        printf("Error al parsear las coordenadas\n");
    }
    return posX;
}

// Función para obtener la posición Y actual
__attribute__((visibility("default"))) float GetPositionY()
{
    FILE *file = fopen("/home/daniel/Escritorio/transform_log.txt", "r");
    if (file == NULL) {
        perror("No se pudo abrir el archivo");
        return posY;
    }

    char line[256];
    char lastLine[256];

    while (fgets(line, sizeof(line), file)) {
        strncpy(lastLine, line, sizeof(lastLine) - 1);
        lastLine[sizeof(lastLine) - 1] = '\0';
    }

    float posX, posY, posZ;
    int numItems = sscanf(lastLine, "posX: %f, posY: %f, posZ: %f", &posX, &posY, &posZ);

    if (numItems == 3) {
        printf("PosX: %f, PosY: %f, PosZ: %f\n", posX, posY, posZ);
    } else {
        printf("Error al parsear las coordenadas\n");
    }


    return posY;
}

// Función para obtener la posición Z actual
__attribute__((visibility("default"))) float GetPositionZ() {

    FILE *file = fopen("/home/daniel/Escritorio/transform_log.txt", "r");
    if (file == NULL) {
        perror("No se pudo abrir el archivo");
        return posZ;
    }

    char line[256];
    char lastLine[256];

    while (fgets(line, sizeof(line), file)) {
        strncpy(lastLine, line, sizeof(lastLine) - 1);
        lastLine[sizeof(lastLine) - 1] = '\0';
    }

    float posX, posY, posZ;
    int numItems = sscanf(lastLine, "posX: %f, posY: %f, posZ: %f", &posX, &posY, &posZ);

    if (numItems == 3) {
        printf("PosX: %f, PosY: %f, PosZ: %f\n", posX, posY, posZ);
    } else {
        printf("Error al parsear las coordenadas\n");
    }
    return posZ;
}

// Función para incrementar la posición en el eje X
__attribute__((visibility("default"))) void incrementXPosition(float increment)
{
    FILE *file = fopen("/home/daniel/Escritorio/transform_log.txt", "r");
    if (file == NULL) {
        perror("No se pudo abrir el archivo");
        return;
    }

    char line[256];
    char lastLine[256];

    while (fgets(line, sizeof(line), file)) {
        strncpy(lastLine, line, sizeof(lastLine) - 1);
        lastLine[sizeof(lastLine) - 1] = '\0';
    }

    fclose(file);

    float posX, posY, posZ;
    int numItems = sscanf(lastLine, "posX: %f, posY: %f, posZ: %f", &posX, &posY, &posZ);

    if (numItems == 3) {
        printf("PosX: %f, PosY: %f, PosZ: %f\n", posX, posY, posZ);
    } else {
        printf("Error al parsear las coordenadas\n");
    }

    posX += increment;

    FILE *logFile = fopen("/home/daniel/Escritorio/transform_log.txt", "a");
    if (logFile) {
        fprintf(logFile, "posX: %f, posY: %f, posZ: %f\n", posX, posY, posZ);
        fclose(logFile);
    }
}

// Función para incrementar la posición en el eje Y
__attribute((visibility("default"))) void incrementYPosition(float increment)
{
    FILE *file = fopen("/home/daniel/Escritorio/transform_log.txt", "r");
    if (file == NULL) {
        perror("No se pudo abrir el archivo");
        return;
    }

    char line[256];
    char lastLine[256];

    while (fgets(line, sizeof(line), file)) {
        strncpy(lastLine, line, sizeof(lastLine) - 1);
        lastLine[sizeof(lastLine) - 1] = '\0';
    }

    fclose(file);

    float posX, posY, posZ;
    int numItems = sscanf(lastLine, "posX: %f, posY: %f, posZ: %f", &posX, &posY, &posZ);

    if (numItems == 3) {
        printf("PosX: %f, PosY: %f, PosZ: %f\n", posX, posY, posZ);
    } else {
        printf("Error al parsear las coordenadas\n");
    }

    posY += increment;

    FILE *logFile = fopen("/home/daniel/Escritorio/transform_log.txt", "a");
    if (logFile) {
        fprintf(logFile, "posX: %f, posY: %f, posZ: %f\n", posX, posY, posZ);
        fclose(logFile);
    }
}

// Función para incrementar la posición en el eje Z
__attribute((visibility("default"))) void incrementZPosition(float increment)
{
    FILE *file = fopen("/home/daniel/Escritorio/transform_log.txt", "r");
    if (file == NULL) {
        perror("No se pudo abrir el archivo");
        return;
    }

    char line[256];
    char lastLine[256];

    while (fgets(line, sizeof(line), file)) {
        strncpy(lastLine, line, sizeof(lastLine) - 1);
        lastLine[sizeof(lastLine) - 1] = '\0';
    }

    fclose(file);

    float posX, posY, posZ;
    int numItems = sscanf(lastLine, "posX: %f, posY: %f, posZ: %f", &posX, &posY, &posZ);

    if (numItems == 3) {
        printf("PosX: %f, PosY: %f, PosZ: %f\n", posX, posY, posZ);
    } else {
        printf("Error al parsear las coordenadas\n");
    }

    posZ += increment;

    FILE *logFile = fopen("/home/daniel/Escritorio/transform_log.txt", "a");
    if (logFile) {
        fprintf(logFile, "posX: %f, posY: %f, posZ: %f\n", posX, posY, posZ);
        fclose(logFile);
    }
}

// Función para guardar el log una vez finalizada la simulación
__attribute((visibility("default"))) void saveLog()
{
    const char* path =   "/home/daniel/Escritorio/transform_log.txt";
    const char *dir = "/home/daniel/Escritorio/logs";

    if (access(path, F_OK) == 0)
    {
        struct stat st;
        if (stat(dir, &st) != 0)
        {
            if (mkdir(dir, 0755) == 0)
            {
                printf("Directorio 'logs' creado correctamente\n");
            }
            else
            {
                perror("Error al crear el directorio 'logs'");
                return;
            }
        }

        time_t t = time(NULL);
        struct tm *tm_info = localtime(&t);
        char newPath[256];
        strftime(newPath, sizeof(newPath), "/home/daniel/Escritorio/logs/log_%Y-%m-%d_%H-%M-%S.txt", tm_info);

        char command[512];
        snprintf(command, sizeof(command), "cp \"%s\" \"%s\"", path, newPath);

        FILE *orig = fopen(path, "rb");
        if (!orig) {
            perror("Error al abrir el archivo de origen");
            return;
        }

        FILE *dest = fopen(newPath, "wb");
        if (!dest) {
            perror("Error al abrir el archivo de destino");
            fclose(orig);
            return;
        }

        char buffer[1024];
        size_t bytes;

        while ((bytes = fread(buffer, 1, sizeof(buffer), orig)) > 0) {
            fwrite(buffer, 1, bytes, dest);
        }

        fclose(orig);
        fclose(dest);

        if (remove(path) == 0)
        {
            printf("Archivo temporal eliminado correctamente");
        }
        else
        {
            perror("No se pudo eliminar el archivo temporal");
        }
    }
    else
    {
        printf("El archivo no se puede acceder o no existe\n");
    }
}
}
