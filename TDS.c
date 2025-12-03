{
  "version": 1,
  "author": "Anonymous maker",
  "editor": "wokwi",
  "parts": [
    {
      "type": "board-esp32-devkit-c-v4",
      "id": "esp",
      "top": 19.2,
      "left": 14.44,
      "attrs": { "builder": "esp-idf" }
    },
    {
      "type": "wokwi-photoresistor-sensor",
      "id": "ldr1",
      "top": 41.6,
      "left": -258.4,
      "attrs": {}
    }
  ],
  "connections": [
    [ "esp:TX", "$serialMonitor:RX", "", [] ],
    [ "esp:RX", "$serialMonitor:TX", "", [] ],
    [ "esp:3V3", "ldr1:VCC", "green", [ "h0" ] ],
    [ "ldr1:GND", "esp:GND.1", "black", [ "h0" ] ]
  ],
  "dependencies": {}
}
