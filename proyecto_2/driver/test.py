import serial

# Configurar el puerto serie
ser = serial.Serial('/dev/arduino0', 9600)  # Ajusta el nombre del puerto según tu configuración
ser.timeout = 1  # Establecer un tiempo de espera para la lectura

try:
    while True:
        # Leer una línea del puerto serie
        line = ser.readline().decode('utf-8').strip()
        
        # Verificar si hay datos
        if line:
            print("Mensaje recibido del puerto serie:", line)
        
except KeyboardInterrupt:
    print("Programa interrumpido.")
    ser.close()  # Cerrar el puerto serie al finalizar
