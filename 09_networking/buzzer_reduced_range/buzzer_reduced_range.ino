// WebSocket code taken from RandomNerd https://randomnerdtutorials.com/esp32-websocket-server-arduino/ + the servo websockets handout code

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

// Pins
int buzz_pin = 25;

// Replace with your network credentials
const char *ssid = "NETWORK";
const char *password = "PASSWORD";

String message = "";
// Allocate the JSON document
JsonDocument doc;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Buzzer notes info. Chose 8 bc it visually fit well on the page
const int n_notes = 8;
int notes[n_notes];

// Timing
int duration_ms = 500;
unsigned long prev_ms = 0;
unsigned long current_ms;

// Tracking song position
bool newRequest = false;
int song_pos = 0;

// Webpage HTML, CSS, JS
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>8 Note Noisemaker</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
  html {
    font-family: Arial, Helvetica, sans-serif;
    text-align: center;
  }
  body {
    margin: 0;
  }
  .content {
    padding: 30px;
    max-width: 90vw;
    margin: 0 auto;
  }
  </style>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" href="data:,">
</head>
<body>
  <div class="content">
    <h4>Select notes to play them on the piezo buzzer</h4>

    <form id="notesForm" accept-charset="utf-8">
        <select id="note0" name="note0" onchange="myNotes.wrangleData()">
            <option value="262">C4</option>
            <option value="294">D4</option>
            <option value="330">E4</option>
            <option value="349">F4</option>
            <option value="392">G4</option>
            <option value="440">A4</option>
            <option value="494">B4</option>
            <option value="523">C5</option>
            <option value="587">D5</option>
            <option value="659">E5</option>
            <option value="698">F5</option>
            <option value="784">G5</option>
            <option value="880">A5</option>
        </select>
        <select id="note1" name="note1" onchange="myNotes.wrangleData()">
            <option value="262">C4</option>
            <option value="294">D4</option>
            <option value="330">E4</option>
            <option value="349">F4</option>
            <option value="392">G4</option>
            <option value="440">A4</option>
            <option value="494">B4</option>
            <option value="523">C5</option>
            <option value="587">D5</option>
            <option value="659">E5</option>
            <option value="698">F5</option>
            <option value="784">G5</option>
            <option value="880">A5</option>
        </select>
        <select id="note2" name="note2" onchange="myNotes.wrangleData()">
            <option value="262">C4</option>
            <option value="294">D4</option>
            <option value="330">E4</option>
            <option value="349">F4</option>
            <option value="392">G4</option>
            <option value="440">A4</option>
            <option value="494">B4</option>
            <option value="523">C5</option>
            <option value="587">D5</option>
            <option value="659">E5</option>
            <option value="698">F5</option>
            <option value="784">G5</option>
            <option value="880">A5</option>
        </select>
        <select id="note3" name="note3" onchange="myNotes.wrangleData()">
            <option value="262">C4</option>
            <option value="294">D4</option>
            <option value="330">E4</option>
            <option value="349">F4</option>
            <option value="392">G4</option>
            <option value="440">A4</option>
            <option value="494">B4</option>
            <option value="523">C5</option>
            <option value="587">D5</option>
            <option value="659">E5</option>
            <option value="698">F5</option>
            <option value="784">G5</option>
            <option value="880">A5</option>
        </select>
        <select id="note4" name="note4" onchange="myNotes.wrangleData()">
            <option value="262">C4</option>
            <option value="294">D4</option>
            <option value="330">E4</option>
            <option value="349">F4</option>
            <option value="392">G4</option>
            <option value="440">A4</option>
            <option value="494">B4</option>
            <option value="523">C5</option>
            <option value="587">D5</option>
            <option value="659">E5</option>
            <option value="698">F5</option>
            <option value="784">G5</option>
            <option value="880">A5</option>
        </select>
        <select id="note5" name="note5" onchange="myNotes.wrangleData()">
            <option value="262">C4</option>
            <option value="294">D4</option>
            <option value="330">E4</option>
            <option value="349">F4</option>
            <option value="392">G4</option>
            <option value="440">A4</option>
            <option value="494">B4</option>
            <option value="523">C5</option>
            <option value="587">D5</option>
            <option value="659">E5</option>
            <option value="698">F5</option>
            <option value="784">G5</option>
            <option value="880">A5</option>
        </select>
        <select id="note6" name="note6" onchange="myNotes.wrangleData()">
            <option value="262">C4</option>
            <option value="294">D4</option>
            <option value="330">E4</option>
            <option value="349">F4</option>
            <option value="392">G4</option>
            <option value="440">A4</option>
            <option value="494">B4</option>
            <option value="523">C5</option>
            <option value="587">D5</option>
            <option value="659">E5</option>
            <option value="698">F5</option>
            <option value="784">G5</option>
            <option value="880">A5</option>
        </select>
        <select id="note7" name="note7" onchange="myNotes.wrangleData()">
            <option value="262">C4</option>
            <option value="294">D4</option>
            <option value="330">E4</option>
            <option value="349">F4</option>
            <option value="392">G4</option>
            <option value="440">A4</option>
            <option value="494">B4</option>
            <option value="523">C5</option>
            <option value="587">D5</option>
            <option value="659">E5</option>
            <option value="698">F5</option>
            <option value="784">G5</option>
            <option value="880">A5</option>
        </select>
        <button type="button" onclick="formatSend();">Play</button>
    </form>

    <div style="display: flex; height: 100%; width: 100%; justify-content: center; align-items: center;">
        <div id="staffSvg" style="height: 40vh; width: 75%; text-align: center">

        </div>
    </div>

    
  </div>
<script>
  var gateway = `ws://${window.location.hostname}/ws`;
  var websocket;
  window.addEventListener('load', onLoad);
  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage; // <-- add this line
  }
  function onOpen(event) {
    console.log('Connection opened');
  }
  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }
  function onMessage(event) {
    /* Visual feedback about currently playing note? */
    console.log(event.data);
    // Thank you loose equality :)
    if (event.data == -1) {
      setTimeout(function() {
        myNotes.syncColor(parseInt(event.data));
    }, 500 * 1.3);
    }
    else {
      myNotes.syncColor(parseInt(event.data));
    }
    
  }
  function onLoad(event) {
    initWebSocket();
  }
    function formatSend() {
        const form = document.getElementById('notesForm');
        const formData = new FormData(form);
        const data = {};
        formData.forEach((value, key) => {
            data[key] = value;
        });
        console.log(JSON.stringify(data));
        websocket.send(JSON.stringify(data));
        //return false;
    }
</script>
<!-- d3  for svg interactivity -->
<script src="https://d3js.org/d3.v7.min.js"></script>
<script src="https://d3js.org/d3-array.v2.min.js"></script>

// Draws the staff and notes, changes notes pos
<script>
    class notesStaff {
        constructor(parentElement, notesData, selectElements) {
            this.parentElement = parentElement;
            this.staffData = [0, 1, 2, 3, 4];
            this.notesData = notesData;
            this.selectElements = selectElements;
            this.frequencies = [ // <-- from Arduino tone() docs
                262,  // C4
                294,  // D4
                330,  // E4
                349,  // F4
                392,  // G4
                440,  // A4
                494,  // B4
                523,  // C5
                587,  // D5
                659,  // E5
                698,  // F5
                784,  // G5
                880   // A5
            ];

            this.initVis();
        }

        initVis() {
            let vis = this; 

            // Margin convention
            let parentWidth = document.getElementById(vis.parentElement).getBoundingClientRect().width;
            let parentHeight = document.getElementById(vis.parentElement).getBoundingClientRect().height;
            
            vis.circleRadius = parentHeight / 14;

            vis.margin = { top: 2 * vis.circleRadius, right: vis.circleRadius, bottom: vis.circleRadius, left: vis.circleRadius};

            vis.width = parentWidth - vis.margin.left - vis.margin.right;
            vis.height = parentHeight - vis.margin.top - vis.margin.bottom;

            // SVG drawing area
            vis.svg = d3.select("#staffSvg").append("svg")
                .attr("width", vis.width + vis.margin.left + vis.margin.right)
                .attr("height", vis.height + vis.margin.top + vis.margin.bottom)
                .append("g")
                .attr("transform", "translate(" + vis.margin.left + "," + vis.margin.top + ")");

            vis.circleRadius = vis.height / 7 / 2;
            
            vis.bars = vis.svg.selectAll(".staffLine")
                .data(vis.staffData)
                .enter()
                .append("line")
                .attr("x1", 0)
                .attr("x2", vis.width)
                .attr("y1", (d, i) =>  vis.circleRadius + i * 2 * vis.circleRadius)
                .attr("y2", (d, i) =>  vis.circleRadius + i * 2 * vis.circleRadius)
                .attr("stroke", "black")
                .attr("stroke-width", 2)
                .attr("class", "staffLine");

            console.log(vis.notesData);

            vis.noteCircles = vis.svg.selectAll(".noteCircle")
                .data(vis.notesData)
                .enter()
                .append("circle")
                .attr("fill", "black")
                .attr("r", vis.circleRadius * 0.9)
                .attr("cx", (d, i) => (vis.width / 8 / 2) * (2 * i + 1))
                .attr("stroke-width", 1)
                .attr("stroke", "black")
                .attr("class", "noteCircle") 
                /*.attr("cy", function (d) {
                    return vis.height - 3 * vis.circleRadius - vis.frequencies.indexOf(d) * vis.circleRadius;
                })*/;

    
            vis.wrangleData();
        }

        // Changes circle y pos
        wrangleData() {
            let vis = this;

            vis.selectElements.forEach((element, i) => {
                // For single-selects, get the value of the currently selected option
                vis.notesData[i]= parseInt(element.value);
            });
            

            console.log(vis.notesData);

            vis.svg.selectAll("circle")
                .data(vis.notesData)
                .attr("cy", function (d) {
                    console.log(d);
                    return vis.height - 3 * vis.circleRadius - vis.frequencies.indexOf(d) * vis.circleRadius;
                });
        }

        // Makes the note red while playing (more or less)
        syncColor(noteNumber) {
          let vis = this;
          
          vis.svg.selectAll("circle")
              .data(vis.notesData)
              .attr("fill", function (d, i) {
                  if (noteNumber == i) {
                      return "red";
                  }
                  else {
                      return "black";
                  }
                  
              })
              .attr("stroke", function (d, i) {
                  if (noteNumber == i) {
                      return "red";
                  }
                  else {
                      return "black";
                  }
                  
              }); 
        }
    }

    // Basically main.js 
    let selectElements = document.querySelectorAll('select');
    let selVals = [];
    selectElements.forEach((element) => {
        // For single-selects, get the value of the currently selected option
        selVals.push(parseInt(element.value));
    });
    let myNotes = new notesStaff("staffSvg", selVals, selectElements);
</script>
</body>
</html>
)rawliteral";

// Initialize WiFi
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    message = (char*)data;
    
    // this is from ArduinoJson docs (https://arduinojson.org/v7/example/parser/)
    const char* json = (char*) data;
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, json);
    // Test if parsing succeeds.
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }

    // Put received notes into notes[] array (and make back into int which I did forget initially)
    String key = "note";
    for (int i = 0; i < n_notes; i++) {
        notes[i] = doc[key + String(i)].as<int>();
    }
    
    Serial.println(message);

    newRequest = true;
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
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

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);
  initWiFi();
  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", index_html);
  });


  noTone(buzz_pin);

  server.begin();

}

void loop() {
  if (newRequest) {
    noTone(buzz_pin);
    song_pos = 0;

    //ws.textAll("playing");
    // Play an annoying little song. Blocked from new song until it's done
    while (song_pos < n_notes) {
        current_ms = millis();
        if (current_ms - prev_ms > duration_ms * 1.3) {
            ws.textAll(String(song_pos));
            Serial.println(notes[song_pos]);
            tone(buzz_pin, notes[song_pos], duration_ms);
            prev_ms = current_ms;
            song_pos++;
        }
    }
  // Need a delay of duration before this goes out. Actually nvm doing this in the js
    ws.textAll(String(-1));

    newRequest = false;
    ws.cleanupClients();
  }
}