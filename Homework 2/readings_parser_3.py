import re
import csv

# Parámetros físicos
MASA_SUSTRATO = 250.0  # g
AGUA_POR_CUCHARADA = 5.0  # g (5 ml ≈ 5 g)

input_file = "pruebas.txt"
output_file = "humedad_relativa.csv"

# Expresiones regulares
re_cucharadas = re.compile(r'(\d+)\s*cucharad', re.IGNORECASE)
re_voltage = re.compile(r'ADC1 Channel\[4\] Cali Voltage:\s+(\d+)\s+mV')

bloques = []
current_cucharadas = None
voltages = []

# --- Parseo ---
with open(input_file, "r", encoding="utf-8") as f:
    for line in f:
        # Detectar cantidad de cucharadas
        match_c = re_cucharadas.search(line)
        if match_c:
            if voltages and current_cucharadas is not None:
                avg_voltage = sum(voltages) / len(voltages)
                bloques.append((current_cucharadas, avg_voltage))
            current_cucharadas = int(match_c.group(1))
            voltages = []
            continue
        
        # Buscar voltajes canal 4
        match_v = re_voltage.search(line)
        if match_v:
            voltages.append(int(match_v.group(1)))

# Guardar último bloque
if voltages and current_cucharadas is not None:
    avg_voltage = sum(voltages) / len(voltages)
    bloques.append((current_cucharadas, avg_voltage))

# --- Calcular humedad física ---
data_hr = []
for cucharadas, voltaje in bloques:
    masa_agua = cucharadas * AGUA_POR_CUCHARADA
    humedad = (masa_agua / MASA_SUSTRATO) * 100
    data_hr.append((humedad, voltaje))

# --- Guardar CSV ---
with open(output_file, "w", newline="", encoding="utf-8") as csvfile:
    writer = csv.writer(csvfile)
    writer.writerow(["Humedad_relativa_%", "Voltaje_mV"])
    for hr, voltaje in data_hr:
        writer.writerow([round(hr, 2), round(voltaje, 2)])

print(f"Archivo CSV generado: {output_file}")
