#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

// Replace with your network credentials
const char* ssid = "iPhone 7";
const char* password = "7378318931";

WebServer server(80);
DHT dht(26, DHT11);

void handleRoot() {
  String html = "<html>\
  <head>\
    <meta name='viewport' content='width=device-width, initial-scale=1'>\
    <link rel='stylesheet' href='https://use.fontawesome.com/releases/v5.7.2/css/all.css' integrity='sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr' crossorigin='anonymous'>\
    <title>ESP32 DHT Server</title>\
    <style>\
      body { font-family: 'Roboto', sans-serif; background: #121212; color: #e0e0e0; margin: 0; display: flex; justify-content: center; align-items: center; height: 100vh; }\
      .container { text-align: center; background: #1e1e1e; padding: 20px; border-radius: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.5); }\
      h2 { margin-bottom: 20px; }\
      .data-box { display: inline-block; margin: 20px; padding: 20px; border: 1px solid #333; border-radius: 10px; background: #2c2c2c; }\
      .dht-labels { font-weight: bold; margin-right: 10px; }\
      .units { font-size: 0.8em; }\
      .updated { margin-top: 20px; font-size: 0.9em; color: #888; }\
      .icon { font-size: 2em; margin-bottom: 10px; }\
      .data { font-size: 1.5em; }\
      @keyframes pulse {\
        0% { transform: scale(1); }\
        50% { transform: scale(1.1); }\
        100% { transform: scale(1); }\
      }\
      .data-box:hover { animation: pulse 1s infinite; }\
    </style>\
    <script>\
      function updateData() {\
        fetch('/data')\
        .then(response => response.json())\
        .then(data => {\
          document.getElementById('temperature').innerHTML = data.temperature.toFixed(2);\
          document.getElementById('humidity').innerHTML = data.humidity.toFixed(2);\
          document.getElementById('timestamp').innerHTML = new Date().toLocaleTimeString();\
        })\
        .catch(error => {\
          console.error('Error fetching data:', error);\
          document.getElementById('temperature').innerHTML = 'Error';\
          document.getElementById('humidity').innerHTML = 'Error';\
        });\
      }\
      setInterval(updateData, 2000);\
    </script>\
  </head>\
  <body>\
    <div class='container'>\
      <h2>ESP32 DHT Server</h2>\
      <div class='data-box'>\
        <div class='icon'>\
          <i class='fas fa-thermometer-half' style='color:#ca3517;'></i>\
        </div>\
        <div class='dht-labels'>Temperature</div>\
        <div class='data' id='temperature'>...</div>\
        <sup class='units'>&deg;C</sup>\
      </div>\
      <div class='data-box'>\
        <div class='icon'>\
          <i class='fas fa-tint' style='color:#00add6;'></i>\
        </div>\
        <div class='dht-labels'>Humidity</div>\
        <div class='data' id='humidity'>...</div>\
        <sup class='units'>&percnt;</sup>\
      </div>\
      <p class='updated'>Last updated: <span id='timestamp'>...</span></p>\
    </div>\
  </body>\
</html>";
  server.send(200, "text/html", html);
}

void handleData() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  String json;
  if (isnan(temperature) || isnan(humidity)) {
    json = "{\"temperature\":\"NaN\",\"humidity\":\"NaN\"}";
  } else {
    json = "{\"temperature\":" + String(temperature) + ",\"humidity\":" + String(humidity) + "}";
  }
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  server.on("/", handleRoot);
  server.on("/data", handleData);

  server.begin();
  Serial.println("HTTP server started");
}


void loop() {
  server.handleClient();
}