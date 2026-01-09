#!/bin/bash
# Script para gerar arquivos WAV de TTS em PT-BR
# Requer: sudo apt install espeak-ng

OUTPUT_DIR="sd_files/tts/frases"
mkdir -p "$OUTPUT_DIR"

# Frases do sistema MorphNode
declare -A PHRASES=(
    ["pronto"]="Sistema pronto"
    ["deauth"]="Deauth rodando"
    ["vou_deauth"]="Vou fazer deauth"
    ["ble"]="BLE spamando"
    ["inicio"]="Ataque iniciado"
    ["parado"]="Ataque parado"
    ["subghz"]="SubGigahertz ativado"
    ["replay"]="Replay ativo"
    ["codigo"]="Código enviado"
    ["relay"]="Relay ativo"
    ["ia"]="IA escolheu"
    ["cartao"]="Cartão lido"
    ["stealth"]="Modo stealth"
    ["bateria"]="Bateria baixa"
    ["wifi"]="WiFi conectado"
    ["brute"]="Brute force completo"
    ["falha"]="Falha injetada"
)

echo "Gerando arquivos WAV em $OUTPUT_DIR..."

for key in "${!PHRASES[@]}"; do
    text="${PHRASES[$key]}"
    output="$OUTPUT_DIR/${key}.wav"
    echo "  $key.wav: \"$text\""
    espeak-ng -v pt-br -s 150 "$text" -w "$output" 2>/dev/null
done

echo ""
echo "Arquivos gerados! Copie a pasta sd_files/ para o cartão SD."
echo "Estrutura:"
echo "  /tts/frases/*.wav"
echo "  /ai/"
echo "  /logs/"
