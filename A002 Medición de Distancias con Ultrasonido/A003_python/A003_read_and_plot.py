from threading import Thread # para poder recibir datos paralelamente a la elaboracion de la grafica, y que no se pare la recoleccion por el hecho de graficar
import serial # comunicacion con el Arduino
import time
# libreria que maneja las queues de forma mas eficiente que la libreria standar de python
# Sirve porque hay datos que van a entrar y salir de la grafica permanentemente
import collections
import matplotlib.pyplot as plt # ploteo
import matplotlib.animation as animation # animar el ploteo
import struct # para desempaquetar los bytes que se leen del Arduino
import pandas as pd # manejo de los datos. no es fundamental pero puede servir de cara al post procesamiento


SERIAL_PORT = '/dev/ttyACM0'
BAUD_RATE = 9600
OUTPUT_FILE = '/home/benattie/Documents/FCEIA/Arduino/A002 Medición de Distancias con Ultrasonido/A003_python/data.csv'


class serialPlot:
    def __init__(self, serialPort = SERIAL_PORT, serialBaud = BAUD_RATE, plotLength = 100, dataNumBytes = 2):
        self.port = serialPort
        self.baud = serialBaud
        self.plotMaxLength = plotLength
        self.dataNumBytes = dataNumBytes
        self.rawData = bytearray(dataNumBytes)
        self.data = collections.deque([0] * plotLength, maxlen=plotLength)
        self.isRun = True
        self.isReceiving = False
        self.thread = None
        self.plotTimer = 0
        self.previousTimer = 0
        self.csvData = []

        print('Trying to connect to: ' + str(serialPort) + ' at ' + str(serialBaud) + ' BAUD.')
        try:
            self.serialConnection = serial.Serial(serialPort, serialBaud, timeout=4)
            print('Connected to ' + str(serialPort) + ' at ' + str(serialBaud) + ' BAUD.')
        except:
            print("Failed to connect with " + str(serialPort) + ' at ' + str(serialBaud) + ' BAUD.')
            raise

    def readSerialStart(self):
        if self.thread == None:
            self.thread = Thread(target=self.backgroundThread)
            self.thread.start()
            # Block till we start receiving values
            while self.isReceiving != True:
                time.sleep(0.1)

    def getSerialData(self, frame, lines, lineValueText, lineLabel, timeText):
        currentTimer = time.perf_counter()
        self.plotTimer = int((currentTimer - self.previousTimer) * 1000)     # the first reading will be erroneous
        self.previousTimer = currentTimer
        timeText.set_text('Plot Interval = ' + str(self.plotTimer) + 'ms')
        value,  = struct.unpack('f', self.rawData)    # use 'h' for a 2 byte integer
        print(value)
        self.data.append(value)    # we get the latest data point and append it to our array
        lines.set_data(range(self.plotMaxLength), self.data)
        lineValueText.set_text('[' + lineLabel + '] = ' + str(value))
        self.csvData.append([currentTimer * 1000, self.data[-1]]) # guardo el valor con su tiempo asociado (tiempo en ms)

    def backgroundThread(self):    # retrieve data
        time.sleep(1.0)  # give some buffer time for retrieving data
        self.serialConnection.reset_input_buffer()
        while (self.isRun):
            self.serialConnection.readinto(self.rawData)
            self.isReceiving = True
            print(self.rawData)

    def close(self):
        self.isRun = False
        self.thread.join()
        self.serialConnection.close()
        print('Disconnected...')
        with open(OUTPUT_FILE, 'w') as f:
            f.write('Tiempo(ms),Distancia(cm)\n')
        df = pd.DataFrame(self.csvData)
        df.to_csv(OUTPUT_FILE, header=False, index=False, mode="a")

def main():
    portName = SERIAL_PORT
    baudRate = BAUD_RATE
    maxPlotLength = 100
    dataNumBytes = 4        # number of bytes of 1 data point
    s = serialPlot(portName, baudRate, maxPlotLength, dataNumBytes)   # initializes all required variables
    s.readSerialStart()                                               # starts background thread

    # plotting starts below
    pltInterval = 50    # Period at which the plot animation updates [ms]
    xmin = 0
    xmax = maxPlotLength
    ymin = -(1)
    ymax = 30
    fig = plt.figure()
    ax = plt.axes(xlim=(xmin, xmax), ylim=(float(ymin - (ymax - ymin) / 10), float(ymax + (ymax - ymin) / 10)))
    ax.set_title('Arduino Digital Read')
    ax.set_xlabel("time")
    ax.set_ylabel("DigitalRead Value")

    lineLabel = 'Distance Value'
    timeText = ax.text(0.50, 0.95, '', transform=ax.transAxes)
    lines = ax.plot([], [], label=lineLabel)[0]
    lineValueText = ax.text(0.50, 0.90, '', transform=ax.transAxes)
    anim = animation.FuncAnimation(fig, s.getSerialData, fargs=(lines, lineValueText, lineLabel, timeText), interval=pltInterval, cache_frame_data=False)    # fargs has to be a tuple

    plt.legend(loc="upper left")
    plt.show()

    s.close()


if __name__ == '__main__':
    main()
