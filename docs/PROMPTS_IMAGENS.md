# 🖼️ Prompts para Geração de Imagens - Lele Origin

Use estes prompts em ferramentas de geração de imagens por IA para criar diagramas técnicos dos componentes.

---

## 📦 Componentes Principais

### 1. CYD-2USB (Placa Principal)

```
Technical pinout diagram of ESP32 CYD-2USB development board, also known as Cheap Yellow Display, with 2.8 inch ILI9341 touchscreen 240x320 resolution. Show the board from top view with the screen visible. Label all GPIO pins on both P1 and P3 headers. Highlight important pins: SPI (MOSI GPIO13, MISO GPIO12, SCK GPIO14), I2C (SDA GPIO21, SCL GPIO22), UART (TX GPIO17, RX GPIO16), touch pins (XPT2046), SD card slot pins. Show the two USB-C connectors. Professional electronics documentation style, clean white background, high detail technical illustration.
```

### 2. PN532 NFC Module V3

```
Technical diagram of PN532 NFC RFID module V3 with red PCB, top view showing the rectangular board with integrated NFC antenna coil visible. Show the DIP switches for I2C/SPI/UART mode selection. Label all pins in order: VCC (3.3V), GND, SDA/TX, SCL/RX, RSTPDN, IRQ, MISO, MOSI, SCK, SS. Show the antenna coil pattern. Include mode selection table: I2C mode (SW1=ON, SW2=OFF), SPI mode (SW1=OFF, SW2=ON). Clean professional electronics documentation style, white background.
```

### 3. DS18B20 Temperature Sensor

```
Technical pinout diagram of DS18B20 digital temperature sensor in TO-92 package. Show front view with the flat side facing viewer where text is printed. Three pins at bottom, labeled left to right: Pin 1 GND (Ground), Pin 2 DQ (Data/1-Wire), Pin 3 VDD (Power 3.3V-5V). Show the characteristic rounded back and flat front of TO-92 package. Include note about 4.7k ohm pull-up resistor required on DQ pin. Clean minimalist style, white background, professional electronics documentation.
```

### 4. WS2812B RGB LED

```
Technical pinout diagram of WS2812B 5050 SMD addressable RGB LED chip. Show top view of the 5mm x 5mm square package with chamfered corner indicating pin 1. Four pins labeled: Pin 1 VDD (5V power, top-left), Pin 2 DOUT (Data Out, top-right), Pin 3 GND (Ground, bottom-right), Pin 4 DIN (Data In, bottom-left). Show the three LED elements (red, green, blue) visible inside the package. Include note about 330 ohm resistor on DIN and 100uF capacitor on power. Clean technical style, white background.
```

### 5. CC1101 RF Module

```
Technical diagram of CC1101 Sub-GHz RF transceiver module with SMA antenna connector. Show top view of green PCB. Label the 8-pin header: VCC (3.3V), GND, MOSI (GPIO23), MISO (GPIO19), SCK (GPIO18), CSN (GPIO5), GDO0 (GPIO4), GDO2 (GPIO2). Show the SMA female connector for external 433MHz antenna. Include chip antenna option. Note: 3.3V only, do not connect to 5V. Professional electronics documentation, white background.
```

### 6. NRF24L01+PA+LNA

```
Technical pinout diagram of NRF24L01+PA+LNA long-range 2.4GHz wireless module with external SMA antenna. Show top view of green PCB with PA/LNA chips visible. 8-pin dual-row header labeled: VCC (3.3V), GND, CE (GPIO22), CSN (GPIO21), SCK (GPIO18), MOSI (GPIO23), MISO (GPIO19), IRQ (optional). Show the SMA antenna connector. Include warning: requires 100uF capacitor between VCC and GND for stability. Clean technical documentation style, white background.
```

### 7. NEO-6M GPS Module

```
Technical diagram of NEO-6M GPS module with ceramic patch antenna. Show top view with the square ceramic antenna visible. Label pins: VCC (3.3V-5V), GND, TX (connects to ESP32 RX GPIO16), RX (connects to ESP32 TX GPIO17). Show the u-blox NEO-6M chip. Include IPEX/u.FL connector for external antenna option. Note default baud rate 9600. Professional electronics style, white background.
```

### 8. YS-IRTM IR Module

```
Technical diagram of YS-IRTM infrared transceiver module. Show both IR LED (transmitter) and IR receiver on the module. Label UART pins: VCC (5V required), GND, TXD (to ESP32 RX), RXD (to ESP32 TX). Note: requires logic level shifter (3.3V to 5V) for ESP32 connection. Show the IR LED dome and receiver window. Clean technical documentation, white background.
```

### 9. 18650 Battery Shield V9

```
Technical diagram of 18650 battery holder shield V9 with 4 battery slots. Show top view with battery positions labeled 1-4. Label output terminals: 5V OUT, 3.3V OUT, GND. Show micro-USB charging input and power switch. Include LED indicators for charging status. Show the boost converter circuit area. Note: supports up to 4x 18650 cells in parallel for extended runtime. Professional electronics style, white background.
```

### 10. SD Card Module

```
Technical pinout diagram of micro SD card module breakout board. Label 6 pins: VCC (3.3V), GND, CS/SS (GPIO5), MOSI (GPIO23), SCK (GPIO18), MISO (GPIO19). Show the micro SD card slot. Note: uses SPI interface, supports up to 32GB cards formatted as FAT32. Clean technical documentation, white background.
```

---

## 🔌 Diagramas de Conexão

### 11. Conexão PN532 ao CYD (I2C)

```
Wiring diagram showing PN532 NFC module connected to ESP32 CYD board via I2C. Show color-coded wires: VCC (red) to 3.3V, GND (black) to GND, SDA (blue) to GPIO21, SCL (yellow) to GPIO22. Show DIP switches set to I2C mode (SW1=ON, SW2=OFF). Include wire length recommendation (keep under 20cm). Clean schematic style with labeled connections, white background.
```

### 12. Conexão CC1101 ao CYD (SPI)

```
Wiring diagram showing CC1101 RF module connected to ESP32 CYD board via SPI. Color-coded wires: VCC (red) to 3.3V, GND (black) to GND, MOSI (green) to GPIO23, MISO (blue) to GPIO19, SCK (yellow) to GPIO18, CSN (orange) to GPIO5, GDO0 (purple) to GPIO4. Show external 433MHz SMA antenna connected. Clean schematic style, white background.
```

### 13. Conexão WS2812B LEDs

```
Wiring diagram showing chain of 4 WS2812B LEDs connected to ESP32 CYD. Show data line with 330 ohm resistor between GPIO16 and first LED DIN. Power rails: 5V (red) and GND (black) to all LEDs. Show 1000uF capacitor across power rails near first LED. Daisy-chain connection: DOUT of LED1 to DIN of LED2, etc. Clean schematic style, white background.
```

### 14. Conexão DS18B20

```
Wiring diagram showing DS18B20 temperature sensor connected to ESP32 CYD in normal power mode. Three wires: VDD (red) to 3.3V, GND (black) to GND, DQ (yellow) to GPIO4. Show 4.7k ohm pull-up resistor between DQ and VDD. Include optional parasite power mode diagram. Clean schematic style, white background.
```

### 15. Sistema Completo

```
Complete system wiring diagram showing all modules connected to ESP32 CYD-2USB board. Include: PN532 (I2C), CC1101 (SPI1), NRF24L01 (SPI2), DS18B20 (1-Wire), WS2812B LEDs (GPIO16), GPS NEO-6M (UART), 18650 battery shield. Use color-coded wires and show power distribution. Label all GPIO connections. Professional schematic style, clean layout, white background.
```

---

## 🛡️ Componentes Passivos

### 16. Resistores

```
Technical illustration showing common resistor values used in the project: 4.7k ohm (for DS18B20 pull-up), 330 ohm (for WS2812B data line), 10k ohm (for general pull-ups). Show axial through-hole resistors with color bands. Include color code chart for each value. Clean educational style, white background.
```

### 17. Capacitores

```
Technical illustration showing capacitors used in the project: 100uF electrolytic (for NRF24 stability), 1000uF electrolytic (for WS2812B power), 100nF ceramic (for decoupling). Show both electrolytic and ceramic types with polarity markings. Include voltage ratings. Clean technical style, white background.
```

### 18. Level Shifter BSS138

```
Technical diagram of BSS138 bidirectional logic level shifter module for 3.3V to 5V conversion. Show 4-channel version with labeled pins: LV (3.3V), HV (5V), GND, and 4 pairs of LV1-HV1 to LV4-HV4. Explain bidirectional operation for I2C/UART. Clean schematic style, white background.
```

---

## 📱 Interface e Telas

### 19. Menu Principal

```
Screenshot mockup of Lele Origin main menu on 2.8 inch 240x320 LCD screen. Dark theme with cyan accent color. Show menu items: WiFi, Bluetooth, RF, NFC, IR, Iluminação, Arquivos, Config. Top bar with battery icon and "Lele" text. Bottom navigation: ANT, OK, PROX. Clean UI design, device frame optional.
```

### 20. Menu Iluminação

```
Screenshot mockup of Lele Origin lighting menu showing LED control options. Dark theme. Menu items: Ligar/Desligar, Cor, Selecionar LED, Efeitos, Brilho, Notificações. Show 4 colored circles representing the 4 WS2812B LEDs. Clean modern UI style.
```

### 21. Tela de Ataque WiFi

```
Screenshot mockup of WiFi attack screen showing active deauth attack. Display: target SSID, channel, packets sent counter updating, signal strength bars. Warning icon and red accent for attack mode. Progress animation. Dark theme with red highlights.
```

---

## 📝 Dicas para Melhores Resultados

1. **Use modelos avançados de IA** - melhor para diagramas técnicos
2. **Regenere várias vezes** - cada geração é diferente
3. **Adicione "white background, high detail, technical illustration"** para diagramas
4. **Para UI mockups**: adicione "clean modern design, dark theme"
5. **Para mascotes**: adicione "kawaii style, transparent background, pixel art"

---

## 🔗 Referências Úteis

- **CYD-2USB**: <https://randomnerdtutorials.com/lvgl-cheap-yellow-display-esp32-2432s028r/>
- **PN532**: <https://www.nxp.com/docs/en/nxp/data-sheets/PN532_C1.pdf>
- **WS2812B**: <https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf>
- **CC1101**: <https://www.ti.com/lit/ds/symlink/cc1101.pdf>
