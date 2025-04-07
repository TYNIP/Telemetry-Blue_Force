#ifndef RECEIVER_H
#define RECEIVER_H

#include <SPI.h>
#include <LoRa.h>

#define SS 5
#define RST 14
#define DIO0 26

void setupReceiver() {
    Serial.println("Iniciando LoRa Receptor...");
    
    LoRa.setPins(SS, RST, DIO0);

    if (!LoRa.begin(433E6)) {
        Serial.println("Error al iniciar LoRa.");
        while (1);
    }

    Serial.println("LoRa iniciado correctamente.");
}

void receiveMessage() {
    int packetSize = LoRa.parsePacket();
    
    if (packetSize) {
        Serial.print("Mensaje recibido: ");
        
        while (LoRa.available()) {
            Serial.print((char)LoRa.read());
        }
        
        Serial.println();
    }
}

#endif
