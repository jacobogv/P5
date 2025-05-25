import numpy as np
import matplotlib.pyplot as plt

# Parámetros
N = 32                     # Tamaño de la tabla
f_muestreo = 44100
f_senal = 440              # Frecuencia deseada de la señal
incremento = f_senal * N / f_muestreo
num_muestras = 60

# Tabla seno
tabla = np.sin(2 * np.pi * np.arange(N) / N)

# Señal generada por acceso a la tabla
index = 0.0
senal = []
for _ in range(num_muestras):
    idx = int(index) % N
    senal.append(tabla[idx])
    index += incremento

# Gráfica
plt.figure(figsize=(10, 4))
plt.plot(np.arange(N), tabla, 'o', label='Valores de la tabla ')
plt.plot(np.arange(num_muestras), senal, 'o', label='Señal generada ')
plt.title("Señal a partir de seno")
plt.xlabel("Índice")
plt.ylabel("Amplitud")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.show()
