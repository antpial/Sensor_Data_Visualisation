import serial
import time
import random
import math

# Ustaw właściwy port socat (drugi koniec wirtualnego portu)
PORT = '/dev/pts/7'
BAUD_RATE = 9600

# Początkowe współrzędne GPS (podaj swoje wartości)
start_lat = 51.106054  # przykładowa szerokość geograficzna
start_lon = 17.060251  # przykładowa długość geograficzna

# Zakresy wartości z configa (pozostałe sensory)
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

# Parametry ruchu
radius = 10  # promień okręgu w metrach
angular_speed_deg_per_sec = 10  # prędkość kątowa w stopniach na sekundę

angle = 0  # początkowy kąt

# Stałe pomocnicze
EARTH_RADIUS = 6371000  # promień Ziemi w metrach

ser = serial.Serial(PORT, BAUD_RATE)
i = 0

print(f"Wysyłanie danych na port {PORT}...")

def generate_circular_coordinates():
    global angle

    # Oblicz przesunięcie w metrach względem środka
    dx = radius * math.cos(math.radians(angle))
    dy = radius * math.sin(math.radians(angle))

    # Zamiana metry → stopnie geograficzne
    delta_lat = dy / EARTH_RADIUS * (180 / math.pi)
    delta_lon = dx / (EARTH_RADIUS * math.cos(math.radians(start_lat))) * (180 / math.pi)

    lat = start_lat + delta_lat
    lon = start_lon + delta_lon

    # Zwiększ kąt do kolejnego kroku (pełne koło: 360 stopni)
    angle = (angle + angular_speed_deg_per_sec) % 360

    return round(lat, 6), round(lon, 6)

try:
    while True:
        # Generowanie współrzędnych spiralnych
        lat, lon = generate_circular_coordinates()
        gps_coords = f"{lat};{lon}"

        # Generowanie pozostałych wartości sensorycznych
        sensor_values = [gps_coords]
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
