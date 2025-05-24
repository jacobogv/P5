% Cargar el archivo WAV
[senal, fs] = audioread('vibra.wav');
t = (0:length(senal)-1)/fs;

% Usar solo un canal si es stereo
senal_mono = senal(:,1);

% Detectar picos (máximos locales)
[peaks, locs] = findpeaks(senal_mono, fs, 'MinPeakDistance', 0.001);

% Verificar que haya al menos 4 picos para graficar tres ciclos
if length(locs) < 4
    error('No se encontraron suficientes picos para graficar tres ciclos.');
end

% Seleccionar tres ciclos (entre el pico 1 y el pico 4)
idx_start = round(locs(1)*fs);
idx_end = round(locs(4)*fs);

% Extraer la señal y tiempo correspondientes
senal_ciclos = senal_mono(idx_start:idx_end);
t_ciclos = (0:length(senal_ciclos)-1)/fs;

% Calcular periodos entre esos 4 picos
periodos = diff(locs(1:4));

% Encontrar periodo mínimo y máximo
[periodo_min, idx_min] = min(periodos);
[periodo_max, idx_max] = max(periodos);

% Graficar los tres ciclos
figure;
plot(t_ciclos, senal_ciclos, 'b-', 'LineWidth', 1.5);
hold on;

% Marcar los picos con círculos rojos
picos_ciclos_t = locs(1:4) - locs(1);  % tiempo relativo dentro del segmento
picos_ciclos_val = peaks(1:4);
plot(picos_ciclos_t, picos_ciclos_val, 'ro', 'MarkerSize', 8, 'LineWidth', 2);

% Mostrar texto con periodo mínimo
texto_min = sprintf('Periodo mínimo: %.6f s', periodo_min);
x_text_min = mean(picos_ciclos_t(idx_min:idx_min+1));
y_text_min = max(picos_ciclos_val(idx_min:idx_min+1)) + 0.05*range(senal_ciclos);
text(x_text_min, y_text_min, texto_min, 'FontSize', 12, 'Color', 'red', 'FontWeight', 'bold');

% Mostrar texto con periodo máximo
texto_max = sprintf('Periodo máximo: %.6f s', periodo_max);
x_text_max = mean(picos_ciclos_t(idx_max:idx_max+1));
y_text_max = y_text_min + 0.1*range(senal_ciclos); % un poco más arriba
text(x_text_max, y_text_max, texto_max, 'FontSize', 12, 'Color', 'blue', 'FontWeight', 'bold');

xlabel('Tiempo (s)');
ylabel('Amplitud');
title('Tres ciclos de vibrato con periodos mínimo y máximo indicados');
grid on;
hold off;
