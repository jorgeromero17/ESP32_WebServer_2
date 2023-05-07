#include "webServerFunctions.h"

void setup(){
  connectToWifi();
  initWebSocket();
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", html);
  });
  server.begin();
}

void loop() {
  ws.cleanupClients();
  if (shouldGetData) {
    shouldGetData = false;
    GetSpreadsheetData();
  }
}
