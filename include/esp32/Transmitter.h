#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <SPI.h>
#include <LoRa.h>

// LoRa pins
#define LORA_SS   5
#define LORA_RST  14
#define LORA_DIO0 26

// ESP32 Serial2 pins - telemetry reception 
#define ESP32_RX_PIN 16
#define ESP32_TX_PIN 17  //send data from transmitter to teensy (Not used)

void setupTransmitter() {
    Serial.println("Iniciando LoRa Transmisor...");

    Serial2.begin(115200, SERIAL_8N1, ESP32_RX_PIN, ESP32_TX_PIN);
    
    LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

    if (!LoRa.begin(433E6)) {
        Serial.println("Error al iniciar LoRa.");
        while (1);
    }

    Serial.println("LoRa iniciado correctamente.");
}

void sendTelemetry() {
    if (Serial2.available()) {
        String telemetry = Serial2.readStringUntil('\n');
        telemetry.trim(); 

        if (telemetry.length() > 0) {
            Serial.print("Datos enviados: ");
            Serial.println(telemetry);

            LoRa.beginPacket();
            LoRa.print(telemetry);
            LoRa.endPacket();
        }
    }
}

#endif
