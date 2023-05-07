//Se llama las librerias necesarias
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>
   
//variables para conectar al wifi
const char* ssid = "********";
const char* password = "********";

//variable necesaria para la peticion HTTP
String GOOGLE_SCRIPT_ID = "AKfycbz6h55bHKLJ8hlaTYGm01azFfmnUxcq4A4CPSjwTTMVMFBbWofRvWXp_52aEO8ACfc98Q";

// define un objeto de tipo WebServer que se ejecuta en el puerto 80
AsyncWebServer server(80);
// define un objeto de tipo WebSocket que se ejecuta en la direccion ws
AsyncWebSocket ws("/ws");

bool shouldGetData = false;

//HTML Y JS
const char html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html lang="en">
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Web Server ESP32</title>
</head>
<body class="" style="background-color: #0c1221;">  
  <section>
  <div class="relative overflow-hidden bg-no-repeat bg-cover bg-center" style="background-image: url('https://i.imgur.com/DjiQW3y.png'); height: 300px;"></div>
  <div class="container mx-auto w-3/4 sm:w-3/5 2xl:w-1/2">
      <div class="text-center">
      <div class="block rounded-xl shadow-xl px-6 py-6 lg:py-12 md:px-6" style="margin-top: -100px; background: rgba(3, 23, 42,0.6); backdrop-filter: blur(30px);">
          <h1 class="text-4xl lg:text-5xl font-bold tracking-tight text-teal-500">Web Server ESP32</h1>
          <h2 class="font-bold text-slate-200 mt-3 text-3xl lg:text-4xl">Read data from Google Sheets</h2>
      </div>
      </div>
  </div>
  </section>
  <section class="flex flex-col items-center mt-12 px-12">
    <button id="button" class="bg-blue-600 px-6 py-3 text-slate-200 rounded-lg font-bold hover:bg-blue-700 disabled:opacity-75">Get Data</button>
    <div class="relative overflow-x-auto shadow-xl rounded-xl mt-4 w-full">
      <table id="table" class="w-full text-sm text-left text-gray-500 dark:text-gray-400">
        <thead class="text-xs text-gray-700 uppercase bg-gray-50 dark:bg-gray-700 dark:text-gray-400">
          <tr>
              <th scope="col" class="px-6 py-3">
                  Date
              </th>
              <th scope="col" class="px-6 py-3">
                  Time
              </th>
              <th scope="col" class="px-6 py-3">
                  Temperature 1
              </th>
              <th scope="col" class="px-6 py-3">
                  Temperature 2
              </th>
              <th scope="col" class="px-6 py-3">
                Temperature 3
              </th>
              <th scope="col" class="px-6 py-3">
                Temperature 4
              </th>
              <th scope="col" class="px-6 py-3">
                Temperature 5
              </th>
          </tr>
        </thead>
        <tbody id="tbody">
        </tbody>
      </table>
    </div>
    <div role="status" class="mt-4 flex flex-col items-center hidden" id="spinner">
      <svg aria-hidden="true" class="w-12 h-12 mr-2 text-gray-200 animate-spin dark:text-gray-600 fill-blue-600" viewBox="0 0 100 101" fill="none" xmlns="http://www.w3.org/2000/svg">
          <path d="M100 50.5908C100 78.2051 77.6142 100.591 50 100.591C22.3858 100.591 0 78.2051 0 50.5908C0 22.9766 22.3858 0.59082 50 0.59082C77.6142 0.59082 100 22.9766 100 50.5908ZM9.08144 50.5908C9.08144 73.1895 27.4013 91.5094 50 91.5094C72.5987 91.5094 90.9186 73.1895 90.9186 50.5908C90.9186 27.9921 72.5987 9.67226 50 9.67226C27.4013 9.67226 9.08144 27.9921 9.08144 50.5908Z" fill="currentColor"/>
          <path d="M93.9676 39.0409C96.393 38.4038 97.8624 35.9116 97.0079 33.5539C95.2932 28.8227 92.871 24.3692 89.8167 20.348C85.8452 15.1192 80.8826 10.7238 75.2124 7.41289C69.5422 4.10194 63.2754 1.94025 56.7698 1.05124C51.7666 0.367541 46.6976 0.446843 41.7345 1.27873C39.2613 1.69328 37.813 4.19778 38.4501 6.62326C39.0873 9.04874 41.5694 10.4717 44.0505 10.1071C47.8511 9.54855 51.7191 9.52689
          55.5402 10.0491C60.8642 10.7766 65.9928 12.5457 70.6331 15.2552C75.2735 17.9648 79.3347 21.5619 82.5849 25.841C84.9175 28.9121 86.7997 32.2913 88.1811 35.8758C89.083 38.2158 91.5421 39.6781 93.9676 39.0409Z" fill="currentFill"/>
      </svg>
      <span class="text-slate-200">Loading data...</span>
    </div>
  </section>
 <script>
    const tbody = document.getElementById('tbody');
    const button = document.getElementById('button');
    const spinner = document.getElementById('spinner');

    const gateway = `ws://${window.location.hostname}:${window.location.port}/ws`;
    let websocket;

    window.addEventListener('load', onLoad);

    function initWebSocket() {
      console.log('Trying to open a WebSocket connection...');
      websocket = new WebSocket(gateway);
      websocket.onopen    = onOpen;
      websocket.onclose   = onClose;
      websocket.onmessage = onMessage;
    }
    function onOpen(event) {
      console.log('Connection opened');
    }
    function onClose(event) {
      console.log('Connection closed');
      setTimeout(initWebSocket, 2000);
    }
    function onMessage(event) {
      const json = JSON.parse(event.data);
      if(event.data == '0'){
        button.disabled = true;
        spinner.classList.remove('hidden');
      }else {
        generateTbody(json.values);
        spinner.classList.add('hidden');
        button.disabled = false;
      }
    }
    function onLoad(event) {
      initWebSocket();
      initDOMFunctions();
    }

    function initDOMFunctions() {
      button.onclick = () => {
        button.disabled = true;
        spinner.classList.remove('hidden');
        websocket.send('1');
      }
    }

    function generateTbody(values) {
        let tbodyView = `
        ${values.map(element => `
          <tr class="bg-white border-b dark:bg-gray-800 dark:border-gray-700 hover:bg-gray-50 dark:hover:bg-gray-600">
                <td scope="row" class="px-6 py-4">
                  ${convertDate(element[0])}
                </td>
                <td class="px-6 py-4 px-6">
                  ${convertTime(element[0])}
                </td>
                <td class="px-6 py-4">
                  ${element[2]}
                </td>
                <td class="px-6 py-4">
                  ${element[3]}
                </td>
                <td class="px-6 py-4">
                  ${element[4]}
                </td>
                <td class="px-6 py-4">
                  ${element[5]}
                </td>
                <td class="px-6 py-4">
                  ${element[6]}
                </td>
              </tr>
          `).join('')}
        `;
        tbody.innerHTML = tbodyView;
      }

    function convertDate(dateStr) {
      const date = new Date(dateStr);
      return convertedDate = date.toLocaleString('es-ES', { day: '2-digit', month: '2-digit', year: 'numeric' }).replace(/\//g, '-');
    }

    function convertTime(timeSrt) {
      const time = new Date(timeSrt);
      return convertedTime = time.toLocaleString('es-ES', { hour: '2-digit', minute: '2-digit', second: '2-digit' });
    }

  </script>
  <script src="https://cdn.tailwindcss.com"></script>
</body>
</html>
)rawliteral";