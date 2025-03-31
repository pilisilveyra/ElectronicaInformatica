#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

// WiFi
const char* ssid = "Grimoldi-2.4G";
const char* password = "Marcelo1969";

// MQTT
const char* mqtt_server = "44.215.196.57"; // IP publica de mi instancia
const char* topic = "mano/dedos"; // topic

WiFiClient espClient;
PubSubClient client(espClient);

// Servos
Servo servoPulgar;
Servo servoIndice;
Servo servoMedio;
Servo servoAnular;
Servo servoMenique;

// Pines correspondientes
const int pinPulgar = 18;
const int pinIndice = 19;
const int pinMedio = 27;
const int pinAnular = 21;
const int pinMenique = 26;

// conectar a wifi
void setup_wifi() {
  delay(10);
  Serial.println("Conectando a WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n WiFi conectado!");
  Serial.print("Dirección IP local: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  Serial.print("Mensaje MQTT recibido: ");
  Serial.println(msg);

   // chequeamos que el mensaje tenga el formato esperado: $xxxxx
  if (msg.length() == 6 && msg[0] == '$') {
    int dedos[5];
    // extraemos los 5 digitos del mensaje (0 o 1) y los guardamos
    for (int i = 0; i < 5; i++) {
      dedos[i] = msg.charAt(i + 1) - '0';  // convierte char a int
    }

    // mover cada servo segun el valor recibido
    servoPulgar.write(dedos[0] == 1 ? 180 : 0);
    servoIndice.write(dedos[1] == 1 ? 180 : 0);
    servoMedio.write(dedos[2] == 1 ? 180 : 0);
    servoAnular.write(dedos[3] == 1 ? 180 : 0);
    servoMenique.write(dedos[4] == 1 ? 180 : 0);
  }
}

// intenta reconectarse al broker MQTT si se pierde la conexion
void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando a MQTT...");
    if (client.connect("ESP32Cliente")) {
      Serial.println("Conectado al broker!");
      client.subscribe(topic);
    } else {
      Serial.print("Fallo, rc=");
      Serial.print(client.state());
      Serial.println(" → reintentando...");
      delay(5000); // esperar antes de reintentar
    }
  }
}

// configuraciones iniciales
void setup() {
  Serial.begin(115200);

  // Configura la frecuencia de cada servo y los conecta a sus pines
  servoPulgar.setPeriodHertz(50);
  servoIndice.setPeriodHertz(50);
  servoMedio.setPeriodHertz(50);
  servoAnular.setPeriodHertz(50);
  servoMenique.setPeriodHertz(50);

  // Asignar pines
  servoPulgar.attach(pinPulgar);
  servoIndice.attach(pinIndice);
  servoMedio.attach(pinMedio);
  servoAnular.attach(pinAnular);
  servoMenique.attach(pinMenique);

  // Conexion Wifi
  setup_wifi();

  // Configuracion MQTT
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}
  // Bucle ppal
void loop() {
  // Si se desconecta del broker, intenta reconectar
  if (!client.connected()) {
    reconnect();
  }
  // Llama a la funcion que escucha nuevos mensajes MQTT
  client.loop();
}
