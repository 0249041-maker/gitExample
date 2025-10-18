# Caesar TCP (Linux ↔ ESP32)

## Estructura
- `linux/`: cliente/servidor TCP en C con CMake + cifrado César (primer byte = shift).
- `esp/`: proyectos ESP-IDF (server y client) en modo station.
- `python/`: sniffer con Scapy que descifra el payload.
- `docs/`: Reporte y capturas de Wireshark.

## Pasos rápidos
1. Compila Linux (`linux/`).
2. Flashea ESP32 (proyectos en `esp/`).
3. Ejecuta pruebas en puerto 3333.
4. Captura con Wireshark y corre el sniffer de Python.
# Update for PR test
