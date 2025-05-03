#include <WiFi.h>

// Defina o nome e a senha da sua rede Wi-Fi
const char* ssid = "jacson";
const char* password = "12345678";

// Cria o servidor na porta 80
WiFiServer server(80);

// Pino onde o sensor LM35 está conectado (ajuste conforme necessário)
const int LM35_Pin = 34;   // Sensor LM35
const int LED_Pin = 14;    // LED conectado ao D14 (GPIO14)

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED_Pin, OUTPUT);       // Define o LED como saída
  digitalWrite(LED_Pin, HIGH);    // Liga o LED permanentemente

  Serial.println("Conectando ao Wi-Fi...");
  WiFi.begin(ssid, password);

  int tentativas = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    tentativas++;
    if (tentativas > 200) {
      Serial.println("\n❌ Não foi possível conectar ao Wi-Fi.");
      return;
    }
  }

  Serial.println("\n✅ Conectado!");
  Serial.print("IP local: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available(); // Espera por um cliente

  if (client) {
    Serial.println("📶 Cliente conectado.");
    while (client.connected()) {
      if (client.available()) {
        String req = client.readStringUntil('\r');
        Serial.println(req);
        client.flush();

        // Lê o valor do sensor LM35
        int analogValue = analogRead(LM35_Pin);

        // Calcula a temperatura usando 3.3V como referência
        float temperatura = ((analogValue * 3.3 / 4095.0) * 100.0) /3;

        // Envia resposta HTML
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("Connection: close");
        client.println();
        client.println("<!DOCTYPE html><html>");
        client.println("<head><meta charset='UTF-8'><title>Temperatura</title></head>");
        client.println("<body><h1>Temperatura Atual</h1>");
        client.print("<p>");
        client.print(temperatura);
        client.println(" °C</p>");
        client.println("<p>💡 LED está sempre ACESO</p>");
        client.println("</body></html>");
        break;
      }
    }
    delay(1);
    client.stop();
    Serial.println("❌ Cliente desconectado.");
  }
}
