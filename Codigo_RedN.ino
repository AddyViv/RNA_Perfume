
#include <math.h>
#include <SoftwareSerial.h>
#define TxD 11
#define RxD 10
#define KEY 9
#define VCC 8

SoftwareSerial Blue(TxD,RxD);

/********************************************************************
 * Configuración de las entradas analógicas para los Sensores MQ
 ********************************************************************/

int valor=0;

int sensorMQ2=A0;
int sensorMQ4=A1;
int sensorMQ7=A2;
int aux;
int sensorValue;

const int NodosEntrada = 3; 
const int NodosOcultos = 10; 
const int NodosSalida = 3;

 
float entradasOriginales[NodosEntrada];

void setup(){



  Blue.begin(9600);
  pinMode(KEY,OUTPUT);
  pinMode(VCC,OUTPUT);
  
  digitalWrite(KEY,HIGH);
  digitalWrite(VCC,LOW);
  
  digitalWrite(VCC,HIGH);
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  
  Serial.begin(9600);
}

void loop(){

  float valoresEntrada[NodosEntrada] = {
      analogRead(sensorMQ2),
      analogRead(sensorMQ4),
      analogRead(sensorMQ7)
  };
  
  for(int i = 0; i < NodosEntrada; i++){
    entradasOriginales[i] = valoresEntrada[i]; //normalizar los datos de entrada
  }
  
  float salida[NodosSalida];
 // float aux1[]={391,590,411};
  
  RedNeuronal(entradasOriginales, salida);

if(salida[0]>0.8)
  {
    Serial.print("Aire Limpio");
    Serial.print("=>");
    for(int i = 0; i < NodosEntrada; i++)
    {
    Serial.print(salida[i]); 
    Serial.print(",");
    digitalWrite(13,HIGH);
    }
  Serial.println("");
  }
if(salida[1]>0.9)
  {
    Serial.print("Perfume Clon");
    Serial.print("=>");
    for(int i = 0; i < NodosEntrada; i++)
    {
    Serial.print(salida[i]); 
    Serial.print(",");
    
    }
  Serial.println("");
    
  }
if(salida[2]>0.8)
{
      Serial.print("Perfume Original");
    Serial.print("=>");
    for(int i = 0; i < NodosEntrada; i++)
    {
    Serial.print(salida[i]); 
    Serial.print(",");
    digitalWrite(12,HIGH);
       }
  Serial.println("");
  
}

 
  

  delay(1000);
  
}

/**********************************************************************************
 * Algoritmo de la Red Neuonal 
***********************************************************************************/
void RedNeuronal (float *valoresEntrada, float *salida){
  
  /********************************************************************
   * Pesos y Bias resultantes del entrenamiento
   * wh - Pesos capa oculta
   * bh - Bias de la capa oculta
   * wy - Pesos de la capa de salida
   * by - Bias de la capa de salida
   * minh, maxh, miny, maxy, minX y maxX - valores minimos y maximos
   * en las capas de entrada, oculta y salida
  *********************************************************************/
float wh[NodosOcultos][NodosEntrada] = {
   {1.64374283595681,0.983008230342571,-1.07442395943816},
{1.88553474731748,1.99907849947983,1.32596180171634},
{-0.805714237330204,-1.88102054058187,2.45448139667425},
{2.45013805703227,-0.190938100655222,4.64360736756765},
{-1.45720433278034,3.8248261535958,-1.17295672807959},
{1.14164474718021,-1.10984162520312,-2.74282894995035},
{0.845682690049474,-0.0371081156681969,-3.23037288066162},
{1.83889162489484,-1.96247940700443,-0.776846341178393},
{-0.808046693198007,0.753617915951581,3.83348059694802},
{0.721839795674238,-1.79995526530712,-2.69201548789467},
  };
 float bh[NodosOcultos] = {-3.92149741195835, -2.35789646722738, 1.33616152241074, 1.09207799510604, -0.118609024946214, 0.375209068721815, 1.59337020328574, 2.09221601727464, -1.15749719993457, 2.80599424251783};
float wy[NodosSalida][NodosOcultos] = {
{1.19055909221755, 0.271509122637748, -0.0358807749030367, -3.56830113971758, 3.8753263044599, 1.93320801869406, -0.0745623080132962, 0.0529966794651558,0.661546476301253, -1.09168731090077},
{-0.40289208241617, -0.554563492293148,  1.07625559055933, -1.41526273569726, -2.99417558650797,  -1.39895115981971, -2.29098048566972, 1.02271965511123,  0.51976355588912, -0.361772336308696},
{0.18647982616069, -1.12161988156051, -1.15838875143156, 5.91519653009515, 0.0393151055698777,  -0.130263694916199,  0.686746716160302,  -0.389548088966535, -2.08107728932709,1.04063863376153}
};

  float by[NodosSalida] = {-1.30650114548832, 0.874919890599493, -0.557624931009559};

  
 float minh[NodosEntrada] = {29,446,196};
  float maxh[NodosEntrada] = {465,725,626};
  float miny = -1;
  float maxy = 1;
  float minX = 0;
  float maxX = 1;
  float yh[NodosOcultos];
  /**********************************************************************
   * Procesamiento de los valores de entrada de acuerdo a los máximos y  mínimos
   **********************************************************************/
  for(int i = 0; i < NodosEntrada; i++){
    valoresEntrada[i] = valoresEntrada[i] - minh[i];
    valoresEntrada[i] = valoresEntrada[i] * (1/(maxh[i] - minh[i]));
    valoresEntrada[i] = valoresEntrada[i] * (maxy - miny);
    valoresEntrada[i] = valoresEntrada[i] + miny;
  }
  /***************************************************************************
   * Calculo de las salidas de la capa oculta utilizando funcion sigmoidal
   ***************************************************************************/
  for(int i = 0; i < NodosOcultos; i++){
    float suma = bh[i];
    for(int j = 0; j < NodosEntrada; j++){
      suma += (valoresEntrada[j] * wh[i][j]);
    }
    yh[i] = (2/(1 + exp(-2*suma)) - 1);
  }
  /***************************************************************************
   * Calculo de las salidas de la capa de salida utilizando funcion sigmoidal
   ***************************************************************************/
  for(int i = 0; i < NodosSalida; i++){
    float suma = by[i];
    for(int j = 0; j < NodosOcultos; j++){
      suma +=  (yh[j] * wy[i][j]);
    }
    salida[i] = (2/(1 + exp(-2*suma)) - 1);
  }
  /**********************************************************************
   * Procesamiento de las salidas de la capa de salida para obtener la salida de la red neuronal
   **********************************************************************/
  for(int i = 0; i < NodosSalida; i++){
    salida[i] = salida[i] - miny;
    salida[i] = salida[i] * (1/(maxy - miny));
    salida[i] = salida[i] * (maxX - minX);
    salida[i] = salida[i] + minX;
  }
}
