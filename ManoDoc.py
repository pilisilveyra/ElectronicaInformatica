import cv2
import paho.mqtt.client as mqtt
from cvzone.HandTrackingModule import HandDetector

BROKER = "44.215.196.57" #el broker usa la IP publica
PORT = 1883
TOPIC = "mano/dedos"

# Crear cliente MQTT y conectar al broker
client = mqtt.Client()
client.connect(BROKER, PORT, 60)

# Iniciar captura de video desde la camara de la compu
cap = cv2.VideoCapture(0)
# Inicializar el detector de manos (solo una mano, con una confianza de 70%)
detector = HandDetector(maxHands=1, detectionCon=0.7)

# Bucle ppal que corre mientras la camara este prendida
while True:
    success, img = cap.read()

    if not success:
        print("Error al abrir la cámara")
        break
    # Detecta manos en la imagen
    hands, bbox = detector.findHands(img)

    if hands:
        fingers = detector.fingersUp(hands[0]) # Devuelve lista de 5 elementos (1 si el dedo está levantado, 0 si no)
        mensaje = f"${''.join(map(str, fingers))}"  # Formatea el mensaje como "$01001"
        print(f"Enviando mensaje MQTT: {mensaje}")
        client.publish(TOPIC, mensaje) # Publica el mensaje al topic MQTT

    # Muestra la imagen en una ventana
    cv2.imshow("Detección de Manos", img)

    # Salir si se presiona la tecla 'q'
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Cuando termina, se liberan los recursos
cap.release() # Cierra la camara
cv2.destroyAllWindows() #Cierra la ventana
client.disconnect() # Desconecta del broker MQTT
