# ESP32 IoT Temperature Monitor

## Componentes de Hardware
- ESP32 DevKit
- Sensor DHT11
- Display OLED SSD1306 (128x64)
- LED Indicador
- Jumpers

## Pinagem
```
ESP32  ->  Componente
GPIO4  ->  DHT11 DATA
GPIO23 ->  LED Indicador
GPIO19 ->  OLED SDA
GPIO5  ->  OLED SCL
3.3V   ->  DHT11 VCC, OLED VCC
GND    ->  DHT11 GND, OLED GND, LED GND
```

## Bibliotecas Necessárias
- WiFi.h
- HTTPClient.h
- DHT.h (Adafruit DHT Sensor Library)
- Adafruit_GFX.h
- Adafruit_SSD1306.h
- time.h

## Funcionalidades
- Leitura de temperatura e umidade
- Display OLED com informações em tempo real
- LED indicador de status WiFi
- Sincronização de horário via NTP
- Envio de dados para Firebase
- Reconexão automática do WiFi
- Offsets de calibração para temperatura e umidade

## Configuração
1. Instale todas as bibliotecas necessárias no Arduino IDE
2. Configure as credenciais WiFi:
   ```cpp
   const char* ssid = "seu_wifi";
   const char* password = "sua_senha";
   ```
3. Configure o endpoint do Firebase:
   ```cpp
   const char* url = "sua_url_do_firebase";
   ```
4. Ajuste os offsets de calibração se necessário:
   ```cpp
   const float tempOffset = 1.104;
   const float umidOffset = 1.825;
   ```

## Intervalos de Tempo
- Timeout WiFi: 30 segundos
- Tentativa de reconexão WiFi: 4 minutos
- Intervalo de envio de dados: 5 segundos
- Intervalo de piscada do LED: 1 segundo

## Status do LED
- Piscando: WiFi conectado
- Apagado: WiFi desconectado

## Display OLED
- Linha 1: Status do WiFi
- Linha 2: Endereço IP
- Linha 3: Temperatura
- Linha 4: Umidade
- Linha 5: Data e Hora
