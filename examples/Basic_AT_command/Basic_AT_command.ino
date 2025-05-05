#include <SoftwareSerial.h>
#include "AT_command.h"  

SoftwareSerial gsmSerial(7, 8);  // RX, TX
AT_command at(gsmSerial);      

void setup() {
    Serial.begin(9600);
    gsmSerial.begin(9600);
    delay(1000);  

    Serial.println("=== Test de comandos AT ===");

    // Ejemplo 1: Solo enviar comando, sin leer respuesta
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

    // Ejemplo 4: Visualización de comandos para depuración
    Serial.println("\nEjemplo 4: Depuración de comandos...");
    Serial.print("TEST (AT+COPS=?): ");
    Serial.println(at.debugATCommand(12, nullptr, AT_command::TEST, true));
    Serial.print("PARAM (ATD=123456789): ");
    Serial.println(at.debugATCommand(11, "123456789", AT_command::NONE, true));
    Serial.print("QUERY (AT+COPS?): ");
    Serial.println(at.debugATCommand(12, nullptr, AT_command::QUERY, true));

    // Ejemplo 5: Usar readATResponse directamente
    Serial.println("\nEjemplo 5: Enviando AT y leyendo respuesta manualmente...");
    at.sendATCommand(0, nullptr, AT_command::NONE, true, 2000, false); // Enviar sin leer
    response = at.readATResponse(2000); // Leer manualmente
    Serial.print("Resultado: ");
    Serial.println(response);
    at.clearStaticBuffer();
}

void loop() {
}