import serial.tools.list_ports
ports = serial.tools.list_ports.comports()

print("Estos son los puertos disponibles")

available_ports = [port.device for port in ports]
numero_puerto=0
for puerto in available_ports:
    print(numero_puerto, available_ports[numero_puerto])
    numero_puerto=numero_puerto+1

puerto_elegido = int(input("Elija puerto "))

SERIAL_PORT = available_ports[puerto_elegido]
BAUD_RATE = 9600
OUTPUT_FILE = '/home/benattie/Documents/FCEIA/Arduino/A002 Medición de Distancias con Ultrasonido/A003_python/data.csv'
SEPARATOR = ","

from serial import Serial
import matplotlib.pyplot as plt
ser = Serial(SERIAL_PORT, BAUD_RATE, timeout=10)

plt.ion()
fig, ax = plt.subplots()
ax.set_title('Light Sensor')
ax.set_xlabel('Time')
ax.set_ylabel('Light Intensity')

x, y = [], []
line, = ax.plot(x, y)

while True:
    try:
        data = ser.readline().decode().strip()
        print(int(data.split(SEPARATOR)[0]),float(data.split(SEPARATOR)[1]))

        x.append(int(data.split(SEPARATOR)[0]))
        ligma=1000-float(data.split(SEPARATOR)[1])
        y.append(ligma)

        line.set_data(x, y)

        ax.relim()
        ax.autoscale_view(True,True,True)

        fig.canvas.draw()
        fig.canvas.flush_events()

    except KeyboardInterrupt:
        ser.close()
        break
