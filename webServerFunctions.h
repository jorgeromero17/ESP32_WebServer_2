#include "globals.h"

// Función para conectarse a la red Wi-Fi especificada
void connectToWifi(){ //Esta funcion esta basada en el ejemplo: https://todomaker.com/blog/conexion-de-esp32-devkit-v1-a-internet/
  // Inicializar la comunicación serial a 9600 baudios
  Serial.begin(115200);
  // Esperar 
  delay(200);
   // Iniciar la conexión a la red Wi-Fi usando las credenciales especificadas
  WiFi.begin(ssid, password);
  Serial.print("\nConnecting to Wi-Fi.");
  // Esperar y verificar el estado de la conexión Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Mostrar mensaje de conexión exitosa y la dirección IP asignada
  Serial.println("\nWi-Fi connection established");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void notifyClients(String newData) {
  ws.textAll(newData);
}

void GetSpreadsheetData(void) {
  HTTPClient http;
  String url="https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?read";

  Serial.println("\nMaking a request: ");
  notifyClients("0");
  http.begin(url.c_str()); 
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  int httpCode = http.GET();
  String data;
    if (httpCode == HTTP_CODE_OK) {
        data = http.getString();
        Serial.print(httpCode);
        notifyClients(data);
      }
    else {
      Serial.println("Error on HTTP request");
    }
  http.end();
}

// Basada en el proyecto https://RandomNerdTutorials.com/esp32-websocket-server-arduino/
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    shouldGetData = true; // Establecer la bandera en verdadero
  }
}

// Basada en el proyecto https://RandomNerdTutorials.com/esp32-websocket-server-arduino/
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}