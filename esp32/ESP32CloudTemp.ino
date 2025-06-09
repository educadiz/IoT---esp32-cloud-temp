#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <time.h>

// ======= CONFIGURAÇÕES =======
#define DHT_PIN 4
#define DHT_TYPE DHT11
#define LED_PIN 23
#define OLED_SDA 19
#define OLED_SCL 5
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define WIFI_TIMEOUT 30000          // 30 segundos
#define WIFI_RETRY_INTERVAL 240000  // 240 segundos
#define POST_INTERVAL 5000          // 5 segundos
#define BLINK_INTERVAL 1000         // 1 segundo

const char* ssid = "Lula_Ladrao";
const char* password = "jetson@2260";
const char* url = "https://us-central1-esp32cloudtemp.cloudfunctions.net/registrarTemperatura/registrar";
                   
// ======= OBJETOS =======
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
DHT dht(DHT_PIN, DHT_TYPE);

bool wifiConectado = false;
unsigned long ultimoBlink = 0;
unsigned long ultimoWifiTentativa = 0;
unsigned long ultimoEnvio = 0;

// ====== Offsets =======
const float tempOffset = 1.104;
const float umidOffset = 1.825;

void conectarWiFi() {
  Serial.print("Tentando conectar ao Wi-Fi... ");
  WiFi.begin(ssid, password);
  unsigned long inicio = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - inicio < WIFI_TIMEOUT) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi conectado!");
    wifiConectado = true;

    // Configura horário NTP (GMT-3 para Brasil)
    configTime(-3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
    Serial.println("Sincronizando horário via NTP...");
    struct tm timeinfo;
    while (!getLocalTime(&timeinfo)) {
      Serial.println("Aguardando sincronização...");
      delay(1000);
    }
    Serial.println("Horário sincronizado.");
  } else {
    Serial.println("\nFalha na conexão Wi-Fi.");
    WiFi.disconnect(true);
    wifiConectado = false;
  }

  ultimoWifiTentativa = millis();
}

String obterDataHora() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    char buffer[30];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", &timeinfo);
    return String(buffer);
  } else {
    return "Sem horario";
  }
}

void enviarParaFirebase(float temperatura, float umidade) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    String json = "{\"temperatura\":";
    json += String(temperatura, 1);
    json += ",\"umidade\":";
    json += String(umidade, 1);
    json += "}";

    int httpResponseCode = http.POST(json);
    if (httpResponseCode > 0) {
      Serial.print("POST enviado. Código de resposta: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Erro no envio HTTP: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("Wi-Fi desconectado. Não foi possível enviar dados.");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Falha ao iniciar o display OLED");
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  dht.begin();

  conectarWiFi();
}

void loop() {
  // LED Pisca se Wi-Fi conectado
  if (wifiConectado && millis() - ultimoBlink >= BLINK_INTERVAL) {
    ultimoBlink = millis();
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  } else if (!wifiConectado) {
    digitalWrite(LED_PIN, LOW);
  }

  // Tentativa de reconexão Wi-Fi
  if (!wifiConectado && millis() - ultimoWifiTentativa >= WIFI_RETRY_INTERVAL) {
    conectarWiFi();
  }

  // Leitura dos sensores
  float temperatura = dht.readTemperature();
  temperatura = temperatura * tempOffset;
  float umidade = dht.readHumidity();
  umidade = umidade * umidOffset;

  // Envio para Firebase
  if (!isnan(temperatura) && !isnan(umidade) && millis() - ultimoEnvio >= POST_INTERVAL) {
    enviarParaFirebase(temperatura, umidade);
    ultimoEnvio = millis();
  }

  // Atualizar display OLED
  display.clearDisplay();
  display.setCursor(0, 0);

  if (wifiConectado) {
    display.println("Wi-Fi: Conectado");
    display.print("IP: ");
    display.println(WiFi.localIP());
  } else {
    display.println("Wi-Fi: Desconectado");
  }

  if (!isnan(temperatura) && !isnan(umidade)) {
    display.print("Temp: ");
    display.print(temperatura);
    display.println(" C");

    display.print("Umid: ");
    display.print(umidade);
    display.println(" %");
  } else {
    display.println("Erro no DHT11!");
  }

  // Data e hora
  display.setCursor(0, 50);
  display.print(obterDataHora());

  display.display();
  delay(1000);
}
