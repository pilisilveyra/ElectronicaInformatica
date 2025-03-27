import cv2
import paho.mqtt.client as mqtt
from cvzone.HandTrackingModule import HandDetector

BROKER = "3.87.232.45" #el broker usa la IP publica
PORT = 1883
TOPIC = "mano/dedos"

client = mqtt.Client()
client.connect(BROKER, PORT, 60)

cap = cv2.VideoCapture(0)
detector = HandDetector(maxHands=1, detectionCon=0.7)

while True:
    success, img = cap.read()

    if not success:
        print("Error al abrir la cámara")
        break

    hands, bbox = detector.findHands(img)

    if hands:
        fingers = detector.fingersUp(hands[0])
        mensaje = f"${''.join(map(str, fingers))}"
        print(f"Enviando mensaje MQTT: {mensaje}")
        client.publish(TOPIC, mensaje)

    cv2.imshow("Detección de Manos", img)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
client.disconnect()
