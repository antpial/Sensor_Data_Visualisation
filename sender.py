import serial
import time
import random

# Ustaw właściwy port socat (drugi koniec wirtualnego portu)
PORT = '/dev/pts/1'
BAUD_RATE = 9600

# Zakresy wartości z configa
SENSOR_RANGES = [
    (0, 30),     # Thermometer (°C)
    (0, 14),     # Ph sensor (V)
    (0, 100),    # Turbidity sensor (TSS)
    (0, 100),    # Quality sensor (ppm)
    (0, 200),    # Depth sensor (cm)
    (18, 26),    # Battery Voltage (V)
    (0, 100),    # Current (A)
    (0, 20)      # Boat Velocity (km/h)
]

ser = serial.Serial(PORT, BAUD_RATE)
i = 0

print(f"Wysyłanie danych na port {PORT}...")

try:
    while True:
        # Generowanie 8 wartości w zadanych zakresach
        sensor_values = []
        for (low, high) in SENSOR_RANGES:
            val = round(random.uniform(low, high), 2)
            sensor_values.append(str(val))

        message = ";" + ";".join(sensor_values)

        if i % 10 == 0:
            message += f"!Log: Pakiet diagnostyczny #{i}"

        message += "\n"
        ser.write(message.encode('utf-8'))
        print(f"Sent: {message.strip()}")

        i += 1
        if i == 100:
            print("Wysłano 100 wiadomości, resetuję licznik.")
            i = 0

        time.sleep(1)

except KeyboardInterrupt:
    ser.close()
    print("Zamknięto port.")
