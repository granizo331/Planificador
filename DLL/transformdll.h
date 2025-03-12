#pragma once
#ifdef __cplusplus

extern "C" {
#endif


// Función para establecer la posición inicial en todos los ejes
__attribute__((visibility("default"))) void SetTransformPosition(float x, float y, float z);


// Función para obtener la posición X actual
__attribute__((visibility("default"))) float GetPositionX();

// Función para obtener la posición Y actual
__attribute__((visibility("default"))) float GetPositionY();

// Función para obtener la posición Z actual
__attribute__((visibility("default"))) float GetPositionZ();

// Función para incrementar la posición en el eje X
__attribute__((visibility("default"))) void incrementXPosition(float increment);

// Función para incrementar la posición en el eje Y
__attribute__((visibility("default"))) void incrementYPosition(float increment);


// Función para incrementar la posición en el eje Z
__attribute__((visibility("default"))) void incrementZPosition(float increment);


// Función para guardar el log una vez finalizada la simulación
__attribute__((visibility("default"))) void saveLog();

#ifdef __cplusplus
}
#endif
