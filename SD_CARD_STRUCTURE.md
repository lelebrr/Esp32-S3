# SD Card Structure v1.0
## Monster S3 - ESP32-S3 Pentest Device

Estrutura de diretórios otimizada para SD card 128GB.
Auto-criada no boot se não existir.

---

## Árvore de Diretórios

```
/sd_card (root)
├── attacks/
│   ├── ble/                    # BLE spam payloads
│   │   ├── spam_names_br.json  # Nomes PT-BR
│   │   └── fake_macs.txt       # MACs para spoofing
│   ├── wifi/
│   │   ├── pmkid_captures/     # Capturas PMKID
│   │   └── evil_twin_templates/# HTML portais falsos
│   ├── nfc/
│   │   ├── default_keys.txt    # Keys Mifare padrão
│   │   └── dumps/              # Dumps de cartões
│   ├── subghz/
│   │   └── captured/           # Códigos RF gravados
│   └── ir/
│       ├── tv_codes/           # Códigos IR TV
│       └── ac_codes/           # Códigos IR Ar Condicionado
├── ai/
│   ├── q_table.bin             # Q-table RL
│   ├── model.tflite            # TinyML (opcional)
│   └── history.json            # Histórico recompensas
├── tts/
│   ├── voices/
│   │   └── pt-br.dat           # eSpeak-ng PT-BR
│   └── phrases/                # Cache WAV dinâmico
├── cve/
│   ├── cve_br_2025.json        # CVEs Brasil
│   └── brute_br.txt            # Wordlist BR (10M+)
├── logs/
│   ├── attacks.log             # Log de ataques
│   ├── battery.log             # Bateria + temp
│   ├── error.log               # Erros/crashes
│   └── boot.log                # Histórico boots
├── config/
│   ├── settings.json           # Configurações gerais
│   └── pinmap.json             # Mapeamento de pinos
└── backup/
    ├── q_table_backup.bin      # Backup Q-table
    └── cve_backup.json         # Backup CVE
```

---

## Descrição das Pastas

| Pasta | Descrição |
|-------|-----------|
| `/attacks/ble/` | Payloads BLE spam (nomes, MACs) |
| `/attacks/wifi/` | Templates Evil Twin, capturas PMKID |
| `/attacks/nfc/` | Keys Mifare, dumps de cartões |
| `/attacks/subghz/` | Códigos SubGHz 433MHz capturados |
| `/attacks/ir/` | Códigos infravermelhos (TV, AC) |
| `/ai/` | Q-table, modelo TinyML, histórico IA |
| `/tts/` | Voz eSpeak PT-BR, cache de frases |
| `/cve/` | Base CVE Brasil, wordlist brute-force |
| `/logs/` | Logs timestamped (DS3231) |
| `/config/` | Configurações do dispositivo |
| `/backup/` | Backups automáticos (cada 10 boots) |

---

## Atualização CVE

No menu: **Settings > Atualizar CVE**

Ou via código:
```cpp
#include "sd_structure.h"

// Requer WiFi conectado
if (WiFi.isConnected()) {
    update_cve_database();
}
```

Fonte: `https://bruteforce-br.fly.dev/lista_br.txt`

---

## Backup Automático

- Q-table backupeada a cada 10 boots
- Backup manual: `backup_q_table()`
- Rollback: copiar `/backup/q_table_backup.bin` → `/ai/q_table.bin`

---

## Verificação de Integridade

SHA256 em payloads críticos:
```cpp
bool ok = verify_payload_sha256("/attacks/nfc/exploit.bin", "a1b2c3...");
if (!ok) {
    // Payload corrompido - não executar
}
```

---

## Cache PSRAM

Arquivos frequentes carregados no PSRAM (8MB):
```cpp
PSRAMCache* cache = cache_to_psram("/ai/q_table.bin");
if (cache) {
    // cache->data, cache->size disponíveis
    // Até 8 arquivos simultâneos, max 1MB cada
}
```

---

## Logs

Formato: `[YYYY-MM-DD HH:MM:SS] mensagem`

```cpp
log_with_timestamp(SD_FILE_LOG_ATTACKS, "BLE Spam started");
log_with_timestamp(SD_FILE_LOG_ERROR, "NFC init failed");
```

---

## Configurações (settings.json)

```json
{
  "mode": "stealth",      // stealth | full | ia
  "volume": 50,           // 0-100
  "cpu_freq_mhz": 240,
  "display_brightness": 80,
  "auto_backup_interval": 10,
  "tts_enabled": true,
  "ai_learning": true,
  "stealth_leds": false
}
```

---

## Copiar Arquivos pro SD Card

1. Conectar SD no PC
2. Copiar conteúdo de `sd_files/` pro root do SD
3. Inserir SD no ESP32-S3
4. Boot - estrutura verificada e complementada automaticamente
