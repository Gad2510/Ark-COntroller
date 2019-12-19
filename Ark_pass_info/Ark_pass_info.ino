#include "Wire.h"

#define trigPin 2
#define echoPin 3
#define BOTTON_A 4
#define BOTTON_B 5
#define BOTTON_C 6
#define X_AX A2
#define Y_AX A3

//Direccion del IC2
#define MPU_ADDR 0x68

//Ratios de conversion
#define A_R 16384.0
#define G_R 131.0

//Conversion de read -> grados
#define RAD_A_DEG 57.295779

struct Vector2{
  float x;
  float y;  
};
struct Vector2int{
  int x;
  int y;  
};

// Valores sin refinar
int16_t AcX, AcY, AcZ, GyX, GyY,GyZ;

//Angulos
float Acc[2];
float Gy[2];
float Angle[2];

long duration;
float distanceA;
int lastD;
Vector2 last;
unsigned char state=0;

void setup() {
  pinMode(BOTTON_A,INPUT_PULLUP);
  pinMode(BOTTON_B,INPUT_PULLUP);
  pinMode(BOTTON_C,INPUT_PULLUP);
  
  pinMode(Y_AX,INPUT);
  pinMode(X_AX,INPUT);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  Serial.begin(9600);
  Wire.begin();           //Iniciando I2C  
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

    lastD=0;
}
unsigned char VerifyConexion(){
  unsigned char state=0;
  char verification='U';
  if(Serial.available())
    state=Serial.read()==verification;

  return state;
}

void loop() {
    Serial.print("A");
    NormalSecuence(); 
}

void NormalSecuence(){
  Vector2int joyStick;
  Vector2 acc;
  acc.x=0;
  acc.y=0;
  joyStick.x=0;
  joyStick.y=0;
  distanceA=0;

  for(int i=0;i<5;i++){
    Vector2 temp=AcelerometerInput();
    acc.x+=temp.x;
    acc.y+=temp.y;
    
    Vector2int tempInt=JoyStick_Btns();
    joyStick.x+=tempInt.x;
    joyStick.y+=tempInt.y;

    distanceA+=EchoInput();
    delay(20);
  }
  float jx=((joyStick.x)/5)-512;
  if(abs(jx)<30)
    jx=0;
    
  jx=jx/1024;

  float jy=((joyStick.y)/5)-512;
  if(abs(jy)<30)
    jy=0;
    
  jy=jy/1024;
  Serial.print("|");Serial.print(jx);
  Serial.print("|");Serial.print(jy);
  
  unsigned char bottonMap=digitalRead(BOTTON_A);
  bottonMap=bottonMap | ((1 & digitalRead(BOTTON_B))<<1);
  bottonMap=bottonMap | ((1 & digitalRead(BOTTON_C))<<2);
  Serial.print("|");Serial.print(bottonMap);
  
  bottonMap=0;
  float ax=(acc.x-last.x)/5;
  float ay=(acc.y-last.y)/5;

  if(abs(ax)<0.4)
    ax=0;

  if(abs(ay)<0.4)
    ay=0;
  
  Serial.print("|");Serial.print(ax);
  Serial.print("|");Serial.print(ay);
  float d=(distanceA/5);
  Serial.print("|");
  Serial.print(d);
  last=acc;
  
  
  Serial.println();
}

Vector2int JoyStick_Btns(){
    
    
    Vector2int results;
    results.x=analogRead(X_AX);
    results.y=analogRead(Y_AX);
    
    return results;
}

Vector2 AcelerometerInput(){
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 6, true);

  AcX=Wire.read()<<8|Wire.read();
  AcY=Wire.read()<<8|Wire.read();
  AcZ=Wire.read()<<8|Wire.read();

  Acc[1]=atan(-1*(AcX/A_R)/sqrt(pow((AcY/A_R),2) + pow((AcZ/A_R),2)))*RAD_TO_DEG;
  Acc[0]= atan((AcY/A_R)/sqrt(pow((AcX/A_R),2) + pow((AcZ/A_R),2)))*RAD_TO_DEG;

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 6, true);
  
  GyX=Wire.read()<<8|Wire.read();
  GyY=Wire.read()<<8|Wire.read();

  Gy[0] = GyX/G_R;
  Gy[1] = GyY/G_R;

  Angle[0] = 0.98 *(Angle[0]+Gy[0]*0.010) + 0.02*Acc[0];
  Angle[1] = 0.98 *(Angle[1]+Gy[1]*0.010) + 0.02*Acc[1];

  Vector2 results;
  results.x=Angle[0];
  results.y=Angle[1];
  return results;
  }

float EchoInput(){
  float distance;
  // Clear the trigPin by setting it LOW:
  digitalWrite(trigPin, LOW);
  
  delayMicroseconds(5);
 // Trigger the sensor by setting the trigPin high for 10 microseconds:
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Read the echoPin. pulseIn() returns the duration (length of the pulse) in microseconds:
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance:
  distance = duration*0.034;
  int d=floor(distance);
  // Print the distance on the Serial Monitor (Ctrl+Shift+M):
  return distance-43;
  }
