import re
import csv

# Archivo de entrada y salida
input_file = "pruebas.txt"
output_file = "humedad_relativa.csv"

# Expresiones regulares
re_cucharadas = re.compile(r'(\d+)\s*cucharad', re.IGNORECASE)
re_voltage = re.compile(r'ADC1 Channel\[4\] Cali Voltage:\s+(\d+)\s+mV')

# Variables
bloques = []
current_cucharadas = None
voltages = []

# --- Parseo del archivo ---
with open(input_file, "r", encoding="utf-8") as f:
    for line in f:
        # Detectar encabezado de cucharadas
        match_c = re_cucharadas.search(line)
        if match_c:
            if voltages and current_cucharadas is not None:
                avg_voltage = sum(voltages) / len(voltages)
                bloques.append((current_cucharadas, avg_voltage))
            current_cucharadas = int(match_c.group(1))
            voltages = []
            continue
        
        # Buscar voltajes del canal 4
        match_v = re_voltage.search(line)
        if match_v:
            voltages.append(int(match_v.group(1)))

# Guardar último bloque
if voltages and current_cucharadas is not None:
    avg_voltage = sum(voltages) / len(voltages)
    bloques.append((current_cucharadas, avg_voltage))

# --- Calcular HR ---
voltajes = [v for _, v in bloques]
v_min = min(voltajes)  # Voltaje a 0 cucharadas
v_max = max(voltajes)  # Voltaje a 20 cucharadas

print(f"Voltaje mínimo (0% HR): {v_min} mV")
print(f"Voltaje máximo (100% HR): {v_max} mV")

# Generar datos con HR
data_hr = []
for cucharadas, v in bloques:
    humedad = (v - v_min) / (v_max - v_min) * 100
    data_hr.append((humedad, v))

# --- Guardar CSV ---
with open(output_file, "w", newline="", encoding="utf-8") as csvfile:
    writer = csv.writer(csvfile)
    writer.writerow(["Humedad_relativa_%", "Voltaje_mV"])
    for hr, voltaje in data_hr:
        writer.writerow([round(hr, 2), round(voltaje, 2)])

print(f"Archivo CSV generado: {output_file}")
