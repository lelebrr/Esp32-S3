# GPS - GY-NEO6MV2 (u-blox NEO-6M)

## Overview

O módulo **GY-NEO6MV2** é um receptor GPS de alta sensibilidade e baixo consumo de energia baseado no chip u-blox NEO-6M. No projeto Monster S3, ele é utilizado para **Wardriving** (monitoramento de redes WiFi/BLE com geolocalização), sincronização de tempo e rastreamento.

## Especificações Técnicas

| Parâmetro | Valor |
|-----------|-------|
| Chip | u-blox NEO-6M |
| Protocolo | NMEA 0183 (UART) |
| Baud Rate | 9600 bps (Padrão) |
| Tensão | 3V ~ 5V |
| Antena | Cerâmica 25x25mm (IPX) |
| TTFF (Cold Start) | 27s |
| Cold Start | 1s |

## Pinagem e Conexão

O módulo utiliza a interface UART2 do ESP32-S3.

| GPS Module | ESP32-S3 GPIO | Função |
|------------|---------------|--------|
| VCC | 3.3V | Alimentação |
| GND | GND | Terra |
| RX | GPIO 18 (TX) | Recebe comandos (opcional) |
| TX | GPIO 17 (RX) | Envia dados NMEA |

> [!IMPORTANT]
> A conexão é cruzada: **TX do GPS vai no RX do ESP32**.

## Funcionalidade: Wardriving

O Monster S3 possui um modo dedicado de Wardriving que varre redes WiFi e registra os dados no cartão SD em formato CSV compatível com ferramentas de análise.

### Como Usar

1. Vá ao menu principal.
2. Selecione **"Wardriving"**.
3. O sistema entrará em modo monitor e começará a registrar.
4. Para parar, use o gesto **WAVE** ou botão **STOP ALL**.

### Formato do Log (`/wardrive_TIMESTAMP.csv`)

O arquivo CSV gerado contém as seguintes colunas:

- Date, Time (UTC)
- Latitude, Longitude, Altitude
- HDOP (Precisão)
- SSID, BSSID
- RSSI (Sinal), Channel, Encryption

### Exportação para KML (Google Earth)

O firmware inclui uma função para converter os logs CSV para KML, permitindo visualização no Google Earth.

```cpp
// Exemplo de uso interno
GPSDriver::exportToKML("/wardrive_12345.csv", "/wardrive_12345.kml");
```

## Sincronização de Tempo

Se o GPS tiver um "fix" válido (3D Fix), o sistema atualizará o relógio interno (e RTC se disponível) para a hora atômica UTC precisa.

## Troubleshooting

| Problema | Solução |
|----------|---------|
| Status "Searching..." eterno | Vá para área externa. O primeiro fix pode levar até 15 min a frio. |
| Dados Corrompidos | Verifique baud rate (9600) e cabos RX/TX. |
| LED piscando | Indica que o módulo tem Fix (sinal válido). |

## Bibliotecas

- **TinyGPS++**: Parsing NMEA.
