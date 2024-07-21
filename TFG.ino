#include "BluetoothSerial.h" //Se añade la librería para la comunicacion por Bluetooth
#include "ESP32Encoder.h" //Se añade la librería para leer encoders

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED) //Verifica si el Bluetooth está habilitado
#error Bluetooth no esta habilitado! Por favor inicia 'make menuconfig' para poder habilitarlo //Genera un error si el Bluetooth no está habilitado
#endif

BluetoothSerial SerialBT; //Crea una instancia para la comunicación por Bluetooth
String BT_data; //Cadena para almacenar los daros recibidos por Bluetooth
char comando; //Variable para almacenar el comando recibido por Bluetooth

ESP32Encoder encoder1; // Crea una instancia para el encoder1
ESP32Encoder encoder2; // Crea una instancia para el encoder2
ESP32Encoder encoder3; // Crea una instancia para el encoder3
ESP32Encoder encoder4; // Crea una instancia para el encoder4
 
long cuentaanterior1 = 0; //Variable para almacenar el contador del encoder1
long cuentaanterior2 = 0; //Variable para almacenar el contador del encoder2
long cuentaanterior3 = 0; //Variable para almacenar el contador del encoder3
long cuentaanterior4 = 0; //Variable para almacenar el contador del encoder4

unsigned long lastTime = 0; //Variable para almacenar el tiempo anterior

const int pulsosporrevolucion = 12; //Constante para los pulsos por revolucion del encoder
const float reduccion = 98.0; //Constante para la revolucion del engranaje
const float Circunferenciarueda = 3.14159 * 0.08; // Circunferencia de la rueda en metros
const float timeInterval = 1.0; // Intervalo de tiempo en segundos

//Diferentes variables de velocidad
int velocidad = 0; 
int velocidadAD =0;
int velocidadAI =0; 
int velocidadBD =0; 
int velocidadBI =0; 

//Configuracion de los pines A y B de cada motor
const int AmotorAD = 16; 
const int BmotorAD = 4; 
const int AmotorAI = 5; 
const int BmotorAI = 17; 
const int AmotorBD = 25; 
const int BmotorBD = 26; 
const int AmotorBI = 32; 
const int BmotorBI = 33;            

// Configuracion de las salidas pwm
const int frecuencia = 10000;
const int resolucion= 8;

void setup() {
  Serial.begin(115200); // Inicializa la comunicación serie
  SerialBT.begin("TFGgerard"); // Inicializa el Bluetooth con el nombre "TFGgerard"

//configuracion PWM para los pines de los motores
  ledcAttach(AmotorAD, frecuencia, resolucion);
  ledcAttach(BmotorAD, frecuencia, resolucion);
  ledcAttach(AmotorAI, frecuencia, resolucion);
  ledcAttach(BmotorAI, frecuencia, resolucion);
  ledcAttach(AmotorBD, frecuencia, resolucion);
  ledcAttach(BmotorBD, frecuencia, resolucion);
  ledcAttach(AmotorBI, frecuencia, resolucion);
  ledcAttach(BmotorBI, frecuencia, resolucion);

//Configuracion de los pines de los encoders
  encoder1.attachHalfQuad(22, 23); 
  encoder2.attachHalfQuad(2, 15); 
  encoder3.attachHalfQuad(34, 35);  
  encoder4.attachHalfQuad(13, 27); 

  //Limpieza de los contadores de los encoders
  encoder1.clearCount();
  encoder2.clearCount();
  encoder3.clearCount();
  encoder4.clearCount();

 parar(); //Se inician los motores en parado
}

void loop() {

   if (SerialBT.available()) { //Verifica si hay datos disponibles en el Bluetooth
    BT_data = SerialBT.readString();// Lee la cadena recibida por Bluetooth
    if (BT_data.length() > 0) {// Verifica si la cadena recibida tiene datos
         if (BT_data.startsWith("<")) {// Verifica si el comando empieza con '<'
         BT_data.remove(0,1);// Elimina el primer carácter
         if(BT_data.indexOf("-")!=-1){// Verifica si hay un '-'
         BT_data.remove(0,((BT_data.indexOf("|")) + 1));// Elimina la parte antes del '|'
         velocidad = (BT_data.toInt());// Convierte la cadena a entero y lo almacena en velocidad
         BT_data.remove(0,((BT_data.indexOf(">")) + 1));// Elimina la parte antes del '>'
         }
         else{
         comando = (BT_data.toInt());// Convierte la cadena a entero y lo almacena en comando
         BT_data.remove(0,((BT_data.indexOf("|")) + 1));// Elimina la parte antes del '|'
         velocidad = (BT_data.toInt());// Convierte la cadena a entero y lo almacena en velocidad
         BT_data.remove(0,((BT_data.indexOf(">")) + 1));// Elimina la parte antes del '>'
         }
      }

//Se ejecutan las funciones según el valor de comando
      switch (comando) {
        case 0: // Parar
          parar();
          break;
        case 1: // Adelante
          adelante();
          break;
        case 2: // Atrás
          atras();
          break;
        case 3: // Derecha
          derecha();
          break;
        case 4: // Izquierda
          izquierda();
          break;        
        case 5: // Diagonal adelante derecha
          diagonalAD();
          break;
        case 6: // Diagonal adelante izquierda
          diagonalAI();
          break;
        case 7: // Diagonal atrás derecha
          diagonalBD();
          break;
        case 8: // Diagonal atrás izquierda
          diagonalBI();
          break;

        default:
          break;
      }

    }

  }

  unsigned long currentTime = millis(); // Obtiene el tiempo actual
if (currentTime - lastTime >= timeInterval * 1000) {// Verifica si ha pasado el intervalo de tiempo
     // Obtiene los contadores actuales de los encoders
    long cuenta1 = encoder1.getCount();
    long cuenta2 = encoder2.getCount();
    long cuenta3 = encoder3.getCount();
    long cuenta4 = encoder4.getCount();

  // Calcula la diferencia de contadores desde la última vez
    long cuentaahora1 = cuenta1 - cuentaanterior1;
    long cuentaahora2 = cuenta2 - cuentaanterior2;
    long cuentaahora3 = cuenta3 - cuentaanterior3;
    long cuentaahora4 = cuenta4 - cuentaanterior4;

// Guarda los contadores actuales para la próxima iteración
    cuentaanterior1 = cuenta1;
    cuentaanterior2 = cuenta2;
    cuentaanterior3 = cuenta3;
    cuentaanterior4 = cuenta4;
   
    // Calcular la velocidad de las ruedas
    float VelocidadRueda1 = calcularVelocidadRueda(cuentaahora1);
    float VelocidadRueda2 = calcularVelocidadRueda(cuentaahora2);
    float VelocidadRueda3 = calcularVelocidadRueda(cuentaahora3);
    float VelocidadRueda4 = calcularVelocidadRueda(cuentaahora4);

    // Enviar velocidades por Bluetooth
    SerialBT.print(VelocidadRueda1);
    SerialBT.print(";");
    SerialBT.print(VelocidadRueda2);
    SerialBT.print(";");
    SerialBT.print(VelocidadRueda3);
    SerialBT.print(";");
    SerialBT.println(VelocidadRueda4);
    SerialBT.print(";");
    lastTime = currentTime; // Actualiza el último tiempo
}
 delay(100);  // Retardo de 100 ms para no sobrecalentar
}

float calcularVelocidadRueda(long cuentaahora) {//Función para el calculo de velocidad de rueda en m/s
    float motorRPM = (cuentaahora/pulsosporrevolucion) * (60/timeInterval);
    float ruedaRPM = motorRPM / reduccion;
    return (ruedaRPM * Circunferenciarueda) / 60.0;
}
void adelante() { //funcion para mover el robot hacia delante
  girodelante(AmotorAD, BmotorAD, velocidad);
  girodelante(AmotorAI, BmotorAI, velocidad);
  girodelante(AmotorBD, BmotorBD, velocidad);
  girodelante(AmotorBI, BmotorBI, velocidad);
}

void atras() { //funcion para mover el robot hacia detras
  girodetras(AmotorAD, BmotorAD, velocidad);
  girodetras(AmotorAI, BmotorAI, velocidad);
  girodetras(AmotorBD, BmotorBD, velocidad);
  girodetras(AmotorBI, BmotorBI, velocidad);
}

void derecha() { //funcion para mover el robot hacia la derecha
 girodelante(AmotorAI, BmotorAI, velocidad);
 girodetras(AmotorAD, BmotorAD, velocidad);
 girodetras(AmotorBI, BmotorBI, velocidad);
 girodelante(AmotorBD, BmotorBD, velocidad);
}

void izquierda() { //funcion para mover el robot hacia la izquierda
 girodetras(AmotorAI, BmotorAI, velocidad);
 girodelante(AmotorAD, BmotorAD, velocidad);
 girodelante(AmotorBI, BmotorBI, velocidad);
 girodetras(AmotorBD, BmotorBD, velocidad);
}
void diagonalAD(){ //funcion para mover el robot hacia la diagonal superior derecha
  velocidadAD=velocidad*cos(41)+velocidad*sin(41);
  velocidadBI=velocidadAD;
  velocidadAI=velocidad*cos(41)-velocidad*sin(41);
  velocidadBD=velocidadAI;
    if(velocidadAI>255){
    velocidadAI=255;
    velocidadBD=255;
  }

  girodelante(AmotorAI, BmotorAI, velocidadAI);
  girodelante(AmotorBD, BmotorBD, velocidadBD);
  girodelante(AmotorAD, BmotorAD, velocidadAD);
  girodelante(AmotorBI, BmotorBI, velocidadBI);

}
void diagonalAI(){ //funcion para mover el robot hacia la diagonal superior izquierda
  velocidadAD=velocidad*cos(41)-velocidad*sin(41);
  velocidadBI=velocidadAD;
    velocidadAI=velocidad*cos(41)+velocidad*sin(41);;
    velocidadBD=velocidadAI;
    if(velocidadAD>255){
    velocidadAD=255;
    velocidadBI=255;
  }

  girodelante(AmotorAI, BmotorAI, velocidadAI);
  girodelante(AmotorBD, BmotorBD, velocidadBD);
  girodelante(AmotorAD, BmotorAD, velocidadAD);
  girodelante(AmotorBI, BmotorBI, velocidadBI);
}
void diagonalBI(){ //funcion para mover el robot hacia la diagonal inferior izquierda
  velocidadAI=velocidad*cos(41)+velocidad*sin(41);
  velocidadBD=velocidadAI;
  velocidadAD=velocidad*cos(41)-velocidad*sin(41);
  velocidadBI=velocidadAD;
    if(velocidadAI>255){
    velocidadAI=255;
    velocidadBD=255;
  }
  girodetras(AmotorAI, BmotorAI, velocidadAI);
  girodetras(AmotorBD, BmotorBD, velocidadBD);
  girodetras(AmotorAD, BmotorAI, velocidadAD);
  girodetras(AmotorBI, BmotorBD, velocidadBI);
}
void diagonalBD(){ //funcion para mover el robot hacia la diagonal inferior derecha
  velocidadAD=velocidad*cos(41)-velocidad*sin(41);
  velocidadBI=velocidadAD;
  velocidadAI=velocidad*cos(41)+velocidad*sin(41);
  velocidadBD=velocidadAI;
  if(velocidadAD>255){
    velocidadBI=255;
    velocidadAD=255;
  }
  girodetras(AmotorAD, BmotorAD, velocidadAD);
  girodetras(AmotorBI, BmotorBI, velocidadBI);
  girodetras(AmotorAI, BmotorAI, velocidadAI);
  girodetras(AmotorBD, BmotorBD, velocidadBD); 
}

void parar() { //funcion para parar el motor
  paroMotor(AmotorAD,BmotorAD);
  paroMotor(AmotorAI,BmotorAI);
  paroMotor(AmotorBD,BmotorBD);
  paroMotor(AmotorBI,BmotorBI);
}

void girodelante(int IN1, int IN2, int vel) {  // Funcion para girar el motor hacia delante  
  ledcWrite(IN1, vel);
  ledcWrite(IN2, 0);
}

void girodetras(int IN1, int IN2, int vel) {  // Funcion para girar el motor hacia detras
  ledcWrite(IN1, 0);
  ledcWrite(IN2, vel);
}

void paroMotor(int IN1, int IN2) {  //Función para parar el motor   
  ledcWrite(IN1, 0);
  ledcWrite(IN2, 0);
}

