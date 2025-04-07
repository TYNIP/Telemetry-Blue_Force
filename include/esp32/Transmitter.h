#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <SPI.h>
#include <LoRa.h>

#define SS 5
#define RST 14
#define DIO0 26

void setupTransmitter() {
    Serial.println("Iniciando LoRa Transmisor...");
    
    LoRa.setPins(SS, RST, DIO0);

    if (!LoRa.begin(433E6)) {
        Serial.println("Error al iniciar LoRa.");
        while (1);
    }

    Serial.println("LoRa iniciado correctamente.");
}

void sendMessage(const char* message) {
    Serial.print("Enviando mensaje: ");
    Serial.println(message);

    LoRa.beginPacket();
    LoRa.print(message);
    LoRa.endPacket();
}

#endif
