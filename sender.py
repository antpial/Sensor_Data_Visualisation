import serial
import time

# Zamień na odpowiedni port z socata, np. /dev/pts/5
PORT = '/dev/pts/4'

ser = serial.Serial(PORT, 9600)
i=0
print(f"Wysyłanie danych na port {PORT}")

try:
    while True:
        message = f";{i};{i+1};{i+2};{i+3};{i+4};{i+5};{i+6}"
        if i % 10 == 0:
            message += f"!Jestem logiem dla paczki {i}"
        message += "\n"
        ser.write(message.encode('utf-8'))
        print(f"Sent: {message.strip()}")
        i += 1
        if i == 100:
            print("Wysłano 100 wiadomości, resetuje.")
            i = 0
        time.sleep(1)
except KeyboardInterrupt:
    ser.close()
    print("Zamknięto port.")
