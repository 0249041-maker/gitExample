import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
from sklearn.metrics import r2_score

# Cargar datos
df = pd.read_csv("humedad_relativa.csv")

x = df["Voltaje_mV"].values
y = df["Humedad_relativa_%"].values

# --- Modelo lineal ---
coef_lineal = np.polyfit(x, y, 1)
poly_lineal = np.poly1d(coef_lineal)
y_lineal = poly_lineal(x)
r2_lineal = r2_score(y, y_lineal)

# --- Modelo logarítmico ---
def log_func(x, a, b):
    return a * np.log(x + 1e-6) + b  # +1e-6 evita log(0)
def exp_func(x, a, b):
    return a * np.exp(b * x)
popt, _ = curve_fit(exp_func, x, y, p0=[1, 0.01], maxfev=10000)
y_exp = exp_func(x, *popt)
r2_exp = r2_score(y, y_exp)

# --- Graficar ---
plt.figure(figsize=(10,6))
plt.scatter(x, y, label="Datos", color="blue")

# Curvas ajustadas
x_fit = np.linspace(min(x), max(x), 200)
plt.plot(x_fit, poly_lineal(x_fit),
         label=f"Lineal: y={coef_lineal[0]:.4f}x+{coef_lineal[1]:.2f} (R²={r2_lineal:.3f})",
         color="red")
plt.plot(x_fit, exp_func(x_fit, *popt),
         label=f"Exponencial: y={popt[0]:.4f}e^({popt[1]:.4f}x) (R²={r2_exp:.3f})",
         color="green")

# Etiquetas
plt.title("Humedad Relativa (%) vs Voltaje (mV)")
plt.xlabel("Voltaje (mV)")
plt.ylabel("Humedad Relativa (%)")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.show()
