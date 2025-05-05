Esta documentación también está disponible en **[Inglés](./README.md)**.

# 🔌 AT_command
## Resumen
`AT_command` es una biblioteca C++ diseñada para enviar y recibir comandos AT a través de una interfaz serial en microcontroladores compatibles con Arduino para módulos GSM. Proporciona funciones para construir, enviar y leer respuestas de comandos AT, con soporte para modificadores (como consulta y prueba), parámetros, y almacenamiento en PROGMEM. 

Diseñada como una biblioteca para su uso en entornos Arduino.

---

## ⚙️ Características

- Construcción y envío de comandos AT con soporte para modificadores (`?` para consulta, `=?` para prueba).
- Lectura de respuestas AT con detección de marcadores personalizados o predeterminados (por ejemplo, `OK`, `ERROR`).
- Almacenamiento de comandos AT en PROGMEM para reducir el uso de RAM.
- Gestión de parámetros dinámicos para comandos como `ATD` (marcar número) o `AT+CMGS` (enviar SMS).
- Depuración de comandos AT mediante generación de cadenas de texto para inspección.
- Soporte para interfaces seriales estándar (`HardwareSerial`) y software (`SoftwareSerial`).
- Manejo robusto de errores, incluyendo validación de índices de comandos y detección de datos corruptos.
- Configuración flexible de tiempos de espera para respuestas.

> La biblioteca simplifica la interacción con módulos GSM al encapsular la lógica de envío y recepción de comandos AT, con un diseño optimizado para entornos con recursos limitados y soporte para depuración en tiempo real.

### 📌 Microcontroladores Compatibles

Compatible con arquitecturas AVR de 8 bits, ARM Cortex-M de 32 bits y Xtensa, siempre que sean compatibles con el IDE de Arduino y dispongan de una interfaz serial (hardware o software).

---

## 📦 Instalación

Opción 1: Copiar los archivos e incluirlos:

1. Copia los siguientes archivos en tu proyecto Arduino:
   - `AT_command.h`
   - `AT_command.cpp`

2. Incluye el encabezado en tu archivo fuente:
   ```cpp
   #include "AT_command.h"
   ```

Opción 2: Instalar la biblioteca e incluirla:

1. Descarga el repositorio como un archivo ZIP.
2. Abre el IDE de Arduino.
3. Ve a **Sketch > Incluir Biblioteca > Añadir biblioteca ZIP** en el menú.
4. Selecciona el archivo ZIP de la biblioteca.
5. Importa la biblioteca:

Para usar la biblioteca en tu sketch, ve a **Sketch > Incluir Biblioteca** y selecciona `AT_command` de la lista para añadir su directiva `#include` a tu código.
```cpp
#include <AT_command.h>
```

---

## 📚 Biblioteca: `AT_command`

### Definiciones y Estructuras Clave

```cpp
const uint8_t TOTAL_COMMANDS = 15;
const uint8_t MAX_COMMAND_LENGTH = 255;
const char atCommands[TOTAL_COMMANDS][MAX_COMMAND_LENGTH] PROGMEM = { ... };
enum ATModifier { NONE, QUERY, TEST };
```

| Definición | Descripción |
|------------|-------------|
| `TOTAL_COMMANDS` | Número total de comandos AT soportados (15). |
| `MAX_COMMAND_LENGTH` | Longitud máxima de una cadena de comando AT (255 caracteres). |
| `atCommands` | Array 2D en PROGMEM que almacena comandos AT predefinidos (por ejemplo, `AT`, `AT+CSQ`, `ATD`). |
| `ATModifier` | Enumera modificadores para comandos AT: `NONE` (sin modificador o con parámetro), `QUERY` (`?`), `TEST` (`=?`). |

---

## 🔓 Funciones Públicas

### Inicialización 

```cpp
AT_command(Stream& serialPort);
```

| Función | Descripción | Parámetros | Retorno |
|---------|-------------|------------|---------|
| `AT_command(Stream& serialPort)` | Construye un objeto `AT_command` con la interfaz serial especificada. | `serialPort (Stream&)`: Puerto serial (por ejemplo, `Serial`, `SoftwareSerial`). | Ninguno |

### Gestión de búferes

```cpp 
void getATCommand(uint8_t index, char* buffer, size_t bufferSize);
void clearStaticBuffer();
void clearBuffer(char* buffer, size_t bufferSize);
```

| Función | Descripción | Parámetros | Retorno |
|---------|-------------|------------|---------|
| `void getATCommand(uint8_t index, char* buffer, size_t bufferSize)` | Recupera un comando AT desde PROGMEM por su índice. | `index (uint8_t)`: Índice del comando<br>`buffer (char*)`: Búfer de destino<br>`bufferSize (size_t)`: Tamaño del búfer | `void` |
| `void clearStaticBuffer()` | Limpia el búfer estático interno usado para respuestas. | Ninguno | `void` |
| `void clearBuffer(char* buffer, size_t bufferSize)` | Limpia un búfer proporcionado. | `buffer (char*)`: Búfer a limpiar<br>`bufferSize (size_t)`: Tamaño del búfer | `void` |

### Construcción y Envío de Comandos

```cpp
void addModifierOrParams(char* sendBuffer, size_t& sendPos, size_t sendBufferSize,
                         char* returnBuffer, size_t& returnPos, size_t returnBufferSize,
                         const char* param, AT_command::ATModifier modifier);
const char* sendATCommand(uint8_t index, const char* param, ATModifier modifier,
                          bool addCR, unsigned long timeout, bool readResponse);
const char* debugATCommand(uint8_t index, const char* param, ATModifier modifier, bool addCR);
```

| Función | Descripción | Parámetros | Retorno |
|---------|-------------|------------|---------|
| `void addModifierOrParams(...)` | Añade modificadores (`?`, `=?`) o parámetros a un comando AT. | `sendBuffer (char*)`: Búfer de comando<br>`sendPos (size_t&)`: Posición en el búfer de envío<br>`sendBufferSize (size_t)`: Tamaño del búfer de envío<br>`returnBuffer (char*)`: Búfer de retorno<br>`returnPos (size_t&)`: Posición en el búfer de retorno<br>`returnBufferSize (size_t)`: Tamaño del búfer de retorno<br>`param (const char*)`: Parámetro opcional<br>`modifier (ATModifier)`: Modificador (`NONE`, `QUERY`, `TEST`) | `void` |
| `const char* sendATCommand(...)` | Envía un comando AT con parámetros y modificadores, opcionalmente lee la respuesta. | `index (uint8_t)`: Índice del comando<br>`param (const char*)`: Parámetro opcional<br>`modifier (ATModifier)`: Modificador<br>`addCR (bool)`: Añadir retorno de carro<br>`timeout (unsigned long)`: Tiempo de espera (ms)<br>`readResponse (bool)`: Leer respuesta | `const char*`: Respuesta en el búfer estático o `nullptr` |
| `const char* debugATCommand(...)` | Genera una cadena de depuración para un comando AT sin enviarlo. | `index (uint8_t)`: Índice del comando<br>`param (const char*)`: Parámetro opcional<br>`modifier (ATModifier)`: Modificador<br>`addCR (bool)`: Añadir retorno de carro | `const char*`: Cadena en el búfer estático |

### Lectura de Respuestas

```cpp
const char* readATResponse(unsigned long timeout = 1000);
const char* readATResponse(unsigned long timeout, const char* endMarkers[], size_t markerCount);
```

| Función | Descripción | Parámetros | Retorno |
|---------|-------------|------------|---------|
| `const char* readATResponse(unsigned long timeout)` | Lee la respuesta AT usando marcadores predeterminados (`OK`, `ERROR`, etc.). | `timeout (unsigned long)`: Tiempo de espera (ms, predeterminado: 1000) | `const char*`: Respuesta en el búfer estático |
| `const char* readATResponse(unsigned long timeout, const char* endMarkers[], size_t markerCount)` | Lee la respuesta AT usando marcadores personalizados. | `timeout (unsigned long)`: Tiempo de espera (ms)<br>`endMarkers (const char*[])`: Array de marcadores<br>`markerCount (size_t)`: Número de marcadores | `const char*`: Respuesta en el búfer estático |

---

## 🧪 Ejemplo de Uso

El siguiente ejemplo demuestra cómo usar la biblioteca `AT_command` para interactuar con un módulo GSM a través de una interfaz `SoftwareSerial`. El código envía varios comandos AT, lee respuestas y utiliza la función de depuración.

```cpp
#include <SoftwareSerial.h>
#include "AT_command.h"

// Configura SoftwareSerial para el módulo GSM
SoftwareSerial gsmSerial(7, 8); // RX, TX
AT_command at(gsmSerial);

void setup() {
  Serial.begin(9600);
  gsmSerial.begin(9600);
  delay(1000); // Espera a que el módulo se inicialice

  Serial.println("=== Prueba de comandos AT ===");

  // Ejemplo 1: Enviar comando AT sin leer respuesta
  Serial.println("Ejemplo 1: Enviando comando AT sin capturar respuesta...");
  at.sendATCommand(0, nullptr, AT_command::NONE, true, 2000, false);
  Serial.println("Comando AT enviado.");

  // Ejemplo 2: Enviar y obtener respuesta (AT+CSDVC)
  Serial.println("\nEjemplo 2: Enviando AT+CSDVC...");
  const char* response = at.sendATCommand(3, nullptr, AT_command::NONE, true, 3000, true);
  Serial.print("Resultado: ");
  Serial.println(response);
  at.clearStaticBuffer();

  // Ejemplo 3: Enviar y obtener respuesta (AT+CSQ)
  Serial.println("\nEjemplo 3: Enviando AT+CSQ...");
  response = at.sendATCommand(5, nullptr, AT_command::NONE, true, 3000, true);
  Serial.print("Resultado: ");
  Serial.println(response);
  at.clearStaticBuffer();

  // Ejemplo 4: Depuración de comandos
  Serial.println("\nEjemplo 4: Depuración de comandos...");
  Serial.print("TEST (AT+COPS=?): ");
  Serial.println(at.debugATCommand(12, nullptr, AT_command::TEST, true));
  Serial.print("PARAM (ATD=123456789): ");
  Serial.println(at.debugATCommand(11, "123456789", AT_command::NONE, true));
  Serial.print("QUERY (AT+COPS?): ");
  Serial.println(at.debugATCommand(12, nullptr, AT_command::QUERY, true));

  // Ejemplo 5: Leer respuesta manualmente
  Serial.println("\nEjemplo 5: Enviando AT y leyendo respuesta manualmente...");
  at.sendATCommand(0, nullptr, AT_command::NONE, true, 2000, false);
  response = at.readATResponse(2000);
  Serial.print("Resultado: ");
  Serial.println(response);
  at.clearStaticBuffer();
}

void loop() {
  // Bucle vacío
}
```
---

## 🧠 Notas de Diseño

- **Optimización de Memoria**: Los comandos AT se almacenan en PROGMEM para minimizar el uso de RAM.
- **Interfaz Serial Flexible**: La biblioteca utiliza la clase base `Stream` para soportar tanto `HardwareSerial` como `SoftwareSerial`, ofreciendo compatibilidad con diversas configuraciones de hardware y permitiendo al usuario elegir la interfaz serial adecuada.
- **Gestión Robusta de Errores**: Incluye validaciones exhaustivas para índices de comandos inválidos, tamaños de búfer, y datos corruptos en las respuestas (caracteres fuera del rango ASCII imprimible).
- **Modificadores Dinámicos**: El soporte para modificadores (`QUERY`, `TEST`) y parámetros dinámicos permite construir comandos AT complejos (por ejemplo, `AT+COPS=?` o `ATD=123456789`) de manera flexible, sin requerir que el usuario formatee manualmente las cadenas.
- **Depuración Simplificada**: La función `debugATCommand` genera representaciones textuales de los comandos AT antes de enviarlos, facilitando la inspección y resolución de problemas durante el desarrollo, especialmente útil para comandos con parámetros o modificadores.
- **Marcadores Personalizables**: La capacidad de definir marcadores de fin de respuesta personalizados (además de los predeterminados como `OK` y `ERROR`) permite manejar respuestas no estándar de módulos GSM.
- **Búfer Estático Eficiente**: Utiliza un búfer estático de 512 bytes para almacenar respuestas, evitando la asignación dinámica de memoria. Se debe limpiar el búfer (`clearStaticBuffer`) entre operaciones.
- **Tiempos de Espera Configurables**: Los tiempos de espera ajustables en `sendATCommand` y `readATResponse`.

## 🧾 Licencia
Este proyecto está licenciado bajo la Licencia MIT — consulta el archivo [LICENSE](https://github.com/ATphonOS/AT_command/blob/main/LICENSE.md) para más detalles.
