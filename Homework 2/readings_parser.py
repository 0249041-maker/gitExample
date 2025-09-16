import re
import csv

# Archivo de entrada y salida
input_file = "pruebas.txt"
output_file = "humedad_vs_mv.csv"
mililiters = 5

# Expresiones regulares
re_cucharadas = re.compile(r'(\d+)\s*cucharad', re.IGNORECASE)
re_voltage = re.compile(r'ADC1 Channel\[4\] Cali Voltage:\s+(\d+)\s+mV')

# Variables para procesamiento
data = []
current_cucharadas = None
voltages = []

with open(input_file, "r", encoding="utf-8") as f:
    for line in f:
        # Detectar encabezado de cucharadas
        match_c = re_cucharadas.search(line)
        if match_c:
            # Si ya tenemos voltajes acumulados, guardamos promedio
            if voltages and current_cucharadas is not None:
                avg_voltage = sum(voltages) / len(voltages)
                data.append((current_cucharadas, avg_voltage))
            # Reiniciar para nuevo bloque
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
    data.append((current_cucharadas, avg_voltage))

# Crear CSV
with open(output_file, "w", newline="", encoding="utf-8") as csvfile:
    writer = csv.writer(csvfile)
    writer.writerow(["Mililitros", "Voltaje_mV"])
    for cucharadas, voltaje in data:
        writer.writerow([(int(cucharadas)*mililiters), round(voltaje, 2)])

print(f"Archivo CSV generado: {output_file}")
