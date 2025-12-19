# 🦆 Lele - Documentação Completa de Scripts BLE (Bad BLE)

> **⚠️ AVISO LEGAL:** Esta documentação e os scripts aqui contidos são exclusivamente para fins educacionais e testes autorizados em ambientes controlados (Red Teaming, Pentest, Labs). O uso indevido é crime.

O Lele utiliza a placa ESP32 para emular um teclado Bluetooth (BLE), permitindo a injeção de comandos (DuckyScript) em dispositivos alvo (Windows, Android, etc.) sem conexão física.

## 📂 Estrutura de Pastas

Os scripts estão organizados em `sd_card/lele/scripts/` nas seguintes categorias:

| Categoria | Caminho | Descrição |
|-----------|---------|-----------|
| **☁️ Entra Cloud** | `entra_cloud/` | Ataques contra Azure AD / Entra ID (Cloud-Only & Hybrid). |
| **🏰 AD Domain** | `ad_domain/` | Ataques contra Active Directory On-Premise. |
| **🪟 Windows** | `windows/` | Configurações, desativação de segurança e ferramentas locais. |
| **🔄 Persistência** | `persistencia/` | Técnicas para manter acesso no alvo. |
| **📤 Exfiltração** | `exfil/` | Extração de dados (senhas, arquivos, tokens). |
| **🔎 Recon** | `recon/` | Coleta de informações do sistema e rede. |
| **🛡️ Bypass** | `bypass/` | Evasão de AMSI, ETW, Antivírus e restrições. |
| **🤡 Prank** | `prank/` | Scripts de "zoeira" para demonstrar impacto visual. |
| **🔐 Senhas** | `senhas/` | Scripts focados em roubo de credenciais locais. |
| **💀 Ransomware** | `ransomware/` | Simulações de criptografia de arquivos. |

---

## 📜 Lista Detalhada de Scripts

### 1. ☁️ Entra Cloud (`entra_cloud/`)

Scripts focados na nuvem Microsoft (Azure/Entra). Requerem, na maioria, que o alvo já esteja logado ou permitem roubo de sessão.

| Arquivo | Descrição | Impacto |
|---------|-----------|---------|
| `full_storm_compromise.txt` | **Combo Final:** Rouba PRT + Registra Device + Ransomware + Backdoor. Simula ataque Storm-0558/0501. | 🔴 Crítico |
| `storm_ransomware_phase1.txt` | Encripta arquivos do OneDrive e exfiltra. | 🔴 Crítico |
| `storm_ransomware_phase2.txt` | Deleta arquivos originais e cria nota de resgate. | 🔴 Crítico |
| `storm_full_exfil.txt` | Exfiltra todos os usuários e grupos do Entra ID. | 🔴 Crítico |
| `entra_prt_steal.txt` | Rouba o **Primary Refresh Token (PRT)** para acesso SSO universal. | 🔴 Crítico |
| `prt_to_token.txt` | Converte um PRT roubado em um Access Token de sessão. | 🔴 Crítico |
| `device_code_phish.txt` | Inicia fluxo de Device Code para phishing de sessão. | 🟠 Alto |
| `device_code_whfb.txt` | Device Code flow focado em Windows Hello for Business. | 🟠 Alto |
| `fido2_key_abuse.txt` | Registra uma chave FIDO2 maliciosa para persistência MFA. | 🟠 Alto |
| `fido2_phish_register.txt` | Phishing para registro de chave FIDO2 legítima do usuário. | 🟠 Alto |
| `entra_token_steal.txt` | Tenta roubar tokens de sessão cacheados no disco/memória. | 🟠 Alto |
| `managed_identity_token.txt` | Rouba token de Identidade Gerenciada (se em VM Azure). | 🟠 Alto |
| `managed_identity_escalate.txt`| Escala privilégios usando Identidade Gerenciada. | 🔴 Crítico |
| `actor_token_sim.txt` | Simula abuso de Actor Token (Cross-Tenant). | 🟠 Alto |
| `external_id_abuse.txt` | Abusa de Entra External ID para escalação cross-tenant. | 🟠 Alto |
| `external_guest_to_internal.txt` | Tenta converter usuário Guest externo em Membro interno. | 🟠 Alto |
| `entra_external_collaboration.txt`| Altera configurações de colaboração externa (backdoor). | 🟠 Alto |
| `entra_guest_escalate.txt` | Tenta escalar de Guest para Global Admin (vulns antigas). | 🔴 Crítico |
| `hybrid_sync_abuse.txt` | Abusa de Entra Connect para resetar senhas da nuvem. | 🔴 Crítico |
| `hybrid_to_cloud_escalate.txt` | Escalação de AD On-Prem para Cloud Global Admin. | 🔴 Crítico |
| `storm0501_pivot.txt` | Pivotamento híbrido estilo Storm-0501 (DCSync em conta de serviço Cloud). | 🔴 Crítico |
| `cloud_only_backdoor.txt` | Cria uma App Registration maliciosa como backdoor. | 🟠 Alto |
| `cloud_only_exfil.txt` | Exfiltra arquivos do OneDrive/SharePoint via token roubado. | 🟠 Alto |
| `azure_cli_persistence.txt` | Cria Service Principal via Azure CLI logado. | 🟠 Alto |
| `consentfix_azurecli.txt` | Abusa de consentimento OAuth via Azure CLI (ConsentFix). | 🟠 Alto |
| `entra_app_consent_grant.txt` | Concede permissões administrativas a um App malicioso. | 🔴 Crítico |
| `app_proxy_abuse.txt` | Cria backdoor de acesso via App Proxy. | 🟠 Alto |
| `entra_device_register.txt` | Registra dispositivo atacante no tenant. | 🟠 Alto |
| `whfb_bypass_register.txt` | Tenta bypass de CA para registrar Windows Hello. | 🟠 Alto |
| `ca_policy_bypass_geo.txt` | Tenta bypass de geolocalização via Token Replay. | 🟠 Alto |
| `ca_bypass_mfa.txt` | Tenta bypass de MFA via autenticação legada. | 🟠 Alto |
| `ca_bypass_app.txt` | Tenta bypass de CA baseada em aplicativo. | 🟠 Alto |
| `ca_evasion_compliant.txt` | Marca dispositivo como "Compliant" falsamente. | 🟠 Alto |
| `ca_policy_delete.txt` | **DESTRUTIVO:** Deleta todas as políticas de Acesso Condicional. | 🔴 Crítico |
| `ca_bypass_signin_logs.txt` | Tenta apagar logs de sign-in do Azure Monitor. | 🟠 Alto |
| `storm_qakbot_deploy.txt` | Simula deploy de malware (Qakbot) via canal Entra. | 🔴 Crítico |
| `entra_serial_console_sim.txt` | Simula abuso de Serial Console em VM Azure. | 🟠 Alto |
| `storm_ransomware_sharepoint.txt`| Encripta sites inteiros do SharePoint. | 🔴 Crítico |

### 2. 🏰 AD Domain (`ad_domain/`)

Ataques clássicos e modernos contra Active Directory local.

| Arquivo | Descrição | Impacto |
|---------|-----------|---------|
| `golden_ticket.txt` | Cria TGT falso (Golden Ticket) para persistência total. | 🔴 Crítico |
| `silver_ticket.txt` | Cria TGS falso (Silver Ticket) para acesso a serviços. | 🔴 Crítico |
| `dcsync.txt` | Simula DC e requisita replicação de senhas (DCSync). | 🔴 Crítico |
| `skeleton_key.txt` | Injeta senha mestra em todos os usuários do DC. | 🔴 Crítico |
| `kerberoasting.txt` | Extrai tickets TGS de serviços para crack offline. | 🟠 Alto |
| `asrep_roasting.txt` | Extrai hashes de usuários sem pré-autenticação Kerberos. | 🟠 Alto |
| `pass_the_hash.txt` | Autentica usando hash NTLM sem saber a senha. | 🟠 Alto |
| `pass_the_ticket.txt` | Injeta ticket Kerberos (.kirbi) na sessão atual. | 🟠 Alto |
| `overpass_the_hash.txt` | Transforma hash NTLM em Ticket Kerberos (TGT). | 🟠 Alto |
| `lateral_movement.txt` | Executa código em máquina remota via WMI. | 🟠 Alto |
| `laps_dump.txt` | Tenta ler senhas de admin local guardadas no LAPS. | 🟠 Alto |
| `gpp_passwords.txt` | Busca senhas legadas em XMLs de Group Policy (SYSVOL). | 🟠 Alto |
| `zerologon_simulado.txt` | Simula exploração da falha Zerologon (CVE-2020-1472). | 🔴 Crítico |
| `printnightmare_2025.txt` | Simula variante recente do PrintNightmare. | 🔴 Crítico |
| `petitpotam.txt` | Coage autenticação do DC para NTLM Relay. | 🟠 Alto |
| `shadowcoerce.txt` | Coage autenticação via MS-FSRVP (VSS). | 🟠 Alto |
| `dfscoerce.txt` | Coage autenticação via DFS-R. | 🟠 Alto |
| `relay_smb.txt` | Configura ataque de SMB Relay com Responder. | 🟠 Alto |
| `adcs_escape.txt` | Explora configurações inseguras de AD CS (Certificados). | 🔴 Crítico |
| `sam_the_admin.txt` | Explora vulnerabilidade CVE-2021-42278 (sAMAccountName). | 🔴 Crítico |
| `no_pac.txt` | Explora vulnerabilidade NoPAC. | 🔴 Crítico |
| `resource_based_constrained.txt`| Configura Resource Based Constrained Delegation (RBCD). | 🟠 Alto |
| `unconstrained_delegation.txt` | Busca servidores com Unconstrained Delegation para captura. | 🟠 Alto |
| `exfil_sam.txt` | Faz dump local dos hives SAM, SYSTEM e SECURITY. | 🔴 Crítico |
| `full_forest_compromise.txt` | Comprometimento total da floresta (Chain de ataques). | 🔴 Crítico |

### 3. 🪟 Windows (`windows/`)

Ferramentas gerais para manipulação do ambiente Windows.

| Arquivo | Descrição |
|---------|-----------|
| `disable_defender_full.txt` | Tenta desativar todas as camadas do Windows Defender (2025). |
| `disable_windows_security.txt`| Desativa a UI do Windows Security via registro. |
| `disable_firewall.txt` | Desliga o Firewall do Windows completamente. |
| `disable_taskmgr.txt` | Desabilita o Gerenciador de Tarefas via registro. |
| `disable_updates.txt` | Para e desabilita o serviço Windows Update. |
| `enable_rdp.txt` | Ativa Remote Desktop e libera no Firewall. |
| `uac_bypass_fodhelper.txt` | Executa comando elevado sem prompt UAC (Fodhelper). |
| `uac_bypass_cmic.txt` | Executa comando elevado sem prompt UAC (CMSTP). |
| `download_execute.txt` | Baixa executável via HTTP e roda. |
| `download_execute_c2.txt` | Variação focada em beacons C2 (Meterpreter/Cobalt). |
| `empty_recycle_bin.txt` | Esvazia lixeira silenciosamente. |

### 4. 🔄 Persistência (`persistencia/`)

Garantindo que o acesso permaneça após reinicialização.

| Arquivo | Descrição |
|---------|-----------|
| `persistence_run.txt` | Adiciona entrada na chave RUN do registro do usuário. |
| `persistence_schtasks.txt` | Cria Tarefa Agendada diária/logon rodando como SYSTEM. |
| `persistence_wmi.txt` | Usa WMI Event Subscription (difícil detecção). |
| `backdoor_user.txt` | Cria usuário Admin local oculto "admin123". |
| `ble_add_admin.txt` | Variação simples de criação de usuário admin. |
| `ble_reverse_shell.txt` | Inicia Reverse Shell PowerShell persistente. |

### 5. 📤 Exfiltração (`exfil/`)

Roubo de dados sensíveis.

| Arquivo | Descrição |
|---------|-----------|
| `ble_wifi_stealer.txt` | Extrai e exfiltra todos os perfis WiFi salvos. |
| `dump_lsass.txt` | Cria dump de memória do processo lsass.exe (Senhas/Hashes). |
| `exfil_chrome_all.txt` | Extrai logins, cookies e histórico do Chrome. |
| `exfil_system_info_webhook.txt`| Envia info detalhada do PC para Webhook. |
| `screenshot_loop.txt` | Tira prints da tela a cada 10s. |
| `ble_keylogger.txt` | Keylogger simples em PowerShell background. |

### 6. 🛡️ Bypass (`bypass/`)

Técnicas para evadir detecção.

| Arquivo | Descrição |
|---------|-----------|
| `amsi_bypass.txt` | Quebra a interface de scan de scripts (AMSI) na memória. |
| `etw_bypass.txt` | Desabilita logs de eventos do Windows (ETW) na memória. |
| `kill_all_av.txt` | Tenta matar processos de AVs conhecidos à força. |
| `ble_disable_defender.txt`| Variação simples de disable defender. |
| `disable_appcontrol.txt` | Desativa AppLocker e WDAC. |
| `disable_edr_hooks.txt` | Tenta remover hooks de EDRs da DLL ntdll.dll (Unhooking). |
| `living_off_land.txt` | Usa binários legítimos (LOLBins) para baixar/rodar malware. |

### 7. 🔎 Recon (`recon/`)

Levantamento de informações.

| Arquivo | Descrição |
|---------|-----------|
| `system_info.txt` | Salva `systeminfo` no Desktop. |
| `network_info.txt` | Salva `ipconfig` e `netstat` no Desktop. |
| `list_users.txt` | Lista usuários e grupos locais. |
| `startup_list.txt` | Lista programas que iniciam com o Windows. |
| `bitlocker_status.txt` | Verifica status de encriptação dos discos. |
| `disk_usage.txt` | Mostra espaço livre/usado. |
| `browser_history_chrome.txt` | Copia histórico do Chrome para análise. |
| `full_domain_recon.txt` | Executa BloodHound collector (SharpHound). |

### 8. 🔐 Senhas (`senhas/`)

Scripts específicos para coleta de credenciais.

| Arquivo | Descrição |
|---------|-----------|
| `ble_wifi_stealer.txt` | (Ref) Roubo de WiFi. |
| `ble_chrome_creds.txt` | (Ref) Senhas do Chrome. |
| `firefox_creds.txt` | Rouba logins salvos do Firefox. |
| `clipboard_stealer.txt` | Monitora e rouba conteúdo copiado (Ctrl+C). |
| `clipboard_logger_webhook.txt`| Envia clipboard para Webhook em tempo real. |

### 9. 🤡 Prank (`prank/`)

Scripts para assustar ou irritar (Inofensivos).

| Arquivo | Descrição |
|---------|-----------|
| `ble_rickroll.txt` | Abre vídeo Never Gonna Give You Up. |
| `wallpaper_rickroll.txt` | Troca o papel de parede. |
| `fake_bsod.txt` | Simula Tela Azul da Morte em tela cheia. |
| `fake_update.txt` | Simula tela de Update do Windows falsa e infinita. |
| `msg_box_spam.txt` | Abre caixas de mensagem infinitas. |
| `ble_hello.txt` | Escreve "Hello World" no bloco de notas. |
| `open_multiple_sites.txt` | Abre 10 abas de sites aleatórios. |
| `volume_max.txt` | Coloca volume no 100% e toca som. |
| `mouse_jiggler.txt` | Mexe o mouse sozinho para evitar lock screen. |
| `random_cd_tray.txt` | Abre/Fecha gaveta de CD (se existir). |
| `ble_shutdown.txt` | Desliga o PC imediatamente. |
| `ble_forkbomb.txt` | Trava o PC abrindo processos infinitos. |
| `invert_screen.txt` | Inverte cores da tela (se suportado). |
| `block_mouse_keyboard.txt` | Bloqueia input do usuário por alguns segundos. |
| `final_prank_combo.txt` | Combo de caos sonoro e visual. |

### 10. 💀 Ransomware (`ransomware/`)

| Arquivo | Descrição |
|---------|-----------|
| `ransomware_simulator.txt` | Script PowerShell educacional que renomeia/encripta arquivos `.txt` no Desktop. **CUIDADO.** |

---

## 🛠️ Notas de Uso

1. **IP e Domínios:** Scripts como `download_execute` ou ataques AD possuem placeholders como `SEU_IP`, `SEUDOMINIO.LOCAL` ou `TARGET_TENANT`. **Edite antes de usar.**
2. **Dependências:** Alguns scripts AD/Cloud tentam baixar ferramentas (`mimikatz.exe`, `Rubeus.exe`, `AADInternals.ps1`) de um servidor HTTP. Você precisa subir um servidor Python (`python -m http.server 80`) hospedando esses arquivos na máquina atacante.
3. **Execução:**
   - Navegue no menu do Lele: `BLE` -> `Bad BLE`.
   - Escolha a categoria.
   - Selecione o script e confirme.
   - O Lele emulará um teclado e digitará o payload rapidamente.

**Bom proveito e Hacking Ético!** 🐉
