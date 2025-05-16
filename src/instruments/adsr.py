import numpy as np
import matplotlib.pyplot as plt

# Función para generar la envolvente ADSR
def generate_adsr(attack, decay, sustain_level, release, sustain_duration=0.5, sample_rate=1000):
    a_samples = int(sample_rate * attack)
    d_samples = int(sample_rate * decay)
    s_samples = int(sample_rate * sustain_duration)
    r_samples = int(sample_rate * release)

    # Fases
    attack_phase = np.linspace(0, 1, a_samples)
    decay_phase = np.linspace(1, sustain_level, d_samples)
    sustain_phase = np.full(s_samples, sustain_level)
    release_phase = np.linspace(sustain_level, 0, r_samples)

    envelope = np.concatenate([attack_phase, decay_phase, sustain_phase, release_phase])
    time = np.linspace(0, len(envelope) / sample_rate, len(envelope))
    return time, envelope

# Configuraciones para los instrumentos
adsr_settings = {
    "Genérico": (0.5, 0.3, 0.6, 0.5, 0.5),            # ataque, caída, sostén, liberación, duración de sostén
    "Percusivo (extinción)": (0.01, 0.4, 0.0, 1.0, 1.0),
    "Percusivo (interrumpido)": (0.01, 0.4, 0.0, 1.0, 0.2),
    "Plano (violín)": (0.1, 0.2, 0.8, 0.2, 0.5),
}

# Crear subgráficas
fig, axs = plt.subplots(2, 2, figsize=(14, 8))
fig.suptitle("Curvas ADSR para Diferentes Tipos de Instrumentos", fontsize=16)

# Graficar cada curva ADSR
for ax, (title, (a, d, s, r, sustain_dur)) in zip(axs.flatten(), adsr_settings.items()):
    t, env = generate_adsr(a, d, s, r, sustain_duration=sustain_dur)
    ax.plot(t, env, label=f"A={a}s, D={d}s, S={s}, R={r}s")
    ax.set_title(title)
    ax.set_xlabel("Tiempo (s)")
    ax.set_ylabel("Amplitud")
    ax.grid(True)
    ax.set_ylim(-0.1, 1.1)
    ax.legend()

plt.tight_layout(rect=[0, 0.03, 1, 0.95])
plt.show()
