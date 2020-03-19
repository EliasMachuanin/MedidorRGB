
struct TSL{double t; double s; double l;};//se define una funcion tipo estructura que nos permite definir y tomar varias variables.
int out = 2; // definicion de salida.
int S0 = 3;
int S1 = 4;
int S2 = 5;
int S3 = 6;
int OE = 7; // salida habilitada.
int LT = 13; // indicador de adquisicion de color segun codificacion RGB.

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2); 


 void setup() {
   TCS3200config();//salta a configurar el sensor.
   Serial.begin(115200);//configura la velocidad del puerto serie.
   Serial.print("\nleyendo sensor 3210\n");//imprime un mensaje.
   delay(100);
   lcd.init();
   lcd.clear();
 }

 void loop() {
   struct TSL tsl = detectorColor();//toma los datos de la funcion de los colores y lo almacena en struct.
   double t = tsl.t;
   Serial.println();
   delay(3000);
   lcd.clear();
 }

 // comando para detectar si hay color alfrente del sensor.
 bool presenteEn(){
  
   //se cataloga el grado de luminosidad segun la hoja de datos.
   //para determinar el grado de luminosdad o oscuridad.
   double presenteEnTolerance = 2;
   // n˙mero se hace grande cuando algo est· en frente del sensor.
   double presenteEn = leerColor(out,0,0)/leerColor(out,0,1);
  
   if(presenteEn < presenteEnTolerance){
     Serial.println("no hay nada en frente del sensor");
     return false;
   }
   return true;
 }

 // retorna TSL segun color detectado.
 //
 struct TSL detectorColor(){
 struct TSL tsl;
 double blanco = leerColor(out,0,1);//out es el pin 2 del arduino.
 double rojoS = leerColor(out,1,1);
 double azulS = leerColor(out,2,1);
 double verdeS = leerColor(out,3,1);
 Serial.print("datos capturados blanco : "); Serial.println(blanco);
 Serial.print("datos capturados R : "); Serial.println(rojoS);
 Serial.print("datos capturados G : "); Serial.println(verdeS);
 Serial.print("datos capturados B : "); Serial.println(azulS);Serial.println();

 double r,v,b; // r,v,b Ä [0..1]
 double offset = 3.0/blanco; // compensacion de luminosidad.
 r=min(1.0, offset+(blanco/rojoS));
 v=min(1.0, offset+(blanco/verdeS));
 b=min(1.0, offset+(blanco/azulS));
 Serial.print(" Rojo : "); Serial.println( r);
 Serial.print(" verde : "); Serial.println( v);
 Serial.print(" azul : "); Serial.println( b);
 Serial.println();
 // transformacion RVB -> TSL.
 // r,v,b Ä [0..1]
 // t Ä [0∞..360∞]; s,l Ä [%]
 double t,s,l;
 double maxRVB = max(max(r, b),v);
 double minRVB = min(min(r, b),v);
 double delta = maxRVB-minRVB;
 double sumatoria = maxRVB+minRVB;
 l=(sumatoria/2.0);
 if(delta==0.0){
  t=s=0.0;
 }
 else{
   //saturacion
   if ( l < 0.5 ) s = delta / sumatoria;
   else s = delta / ( 2.0 - delta );
  
   // tinte
   double del_R = ( ( ( maxRVB - r ) / 6.0 ) + ( delta / 2.0 ) ) / delta;
   double del_G = ( ( ( maxRVB - v ) / 6.0 ) + ( delta / 2.0 ) ) / delta;
   double del_B = ( ( ( maxRVB - b ) / 6.0 ) + ( delta / 2.0 ) ) / delta;
  
   if ( r == maxRVB ) t = del_B - del_G;
   else if ( v == maxRVB ) t = ( 1.0 / 3.0 ) + del_R - del_B;
   else if ( b == maxRVB ) t = ( 2.0 / 3.0 ) + del_G - del_R;
  
   if ( t < 0 ) t += 1.0;
   if ( t > 1 ) t -= 1.0;
 }

 // normalizacion
 t*=360.0; // [0∞..360∞]
 s*=100.0; // [%]
 l*=100.0; // [%]
 tsl.t=t; tsl.s=s; tsl.l=l;

 Serial.print(" tinte [0..360]: "); Serial.println(t);
 Serial.print(" saturacion [%]: "); Serial.println(s);
 Serial.print(" luminancia [%]: "); Serial.println(l);
 
 unsigned int rojo, verde ,azul;
 int html[6];
 if (t>=0&&t<=60){
      rojo=255;
      azul=0;
      verde= (t*64)/(15.01);
 }
 if (t>=60&&t<=120){
      rojo=((t-120)*64)/(105.01-120);
      azul=0;
      verde= 255;
 }
 if (t>=120&&t<=180){
      rojo=0;
      azul=((t-120)*64)/(135.01-120);
      verde= 255;
 } 
 if (t>=180&&t<=240){
      rojo= 0;
      azul= 255;
      verde= ((t-240)*64)/(225.01-240);
 }
  if (t>=240&&t<=300){
      rojo=((t-240)*64)/(255.01-240);
      azul= 255;
      verde= 0;
 }
 if (t>=300&&t<=360){
      rojo= 255;
      azul= ((t-360)*64)/(345.01-360);
      verde= 0;
  }
 
  for(int i=0;i<2;i++){
    if(i==0){
      html[1]=rojo%16;
      html[3]=verde%16;
      html[5]=azul%16;
    }
    if(i==1){
      html[0]=rojo/16;
      html[2]=verde/16;
      html[4]=azul/16;
    }
   }
  char html2[6];
  Serial.println();
  Serial.print("Color: #");
  lcd.setCursor(0,3);
   for(int i=0;i<6;i++){
    switch(html[i]){
      case 0:
       html2[i]='0';
        Serial.print(0);
        break;
      case 1:
      html2[i]='1';
        Serial.print(1);
        break;
      case 2:
      html2[i]='2';
        Serial.print(2);
        break;
      case 3:
      html2[i]='3';
        Serial.print(3);
        break;
      case 4:
      html2[i]='4';
        Serial.print(4);
        break;
      case 5:
      html2[i]='5';
        Serial.print(5);
        break;
      case 6:
      html2[i]='6';
        Serial.print(6);
        break;
      case 7:
      html2[i]='7';
        Serial.print(7);
        break;
      case 8:
      html2[i]='8';
        Serial.print(8);
        break;
      case 9:
      html2[i]='9';
        Serial.print(9);
        break;
      case 10:
      html2[i]='A';
        Serial.print("A");
        break;
      case 11:
      html2[i]='B';
        Serial.print("B");
        break;
      case 12:
      html2[i]='C';
        Serial.print("C");
        break;
      case 13:
      html2[i]='D';
        Serial.print("D");
        break;     
      case 14:
      html2[i]='E';
        Serial.print("E");
        break;
      case 15:    
      html2[i]='F';  
        Serial.print("F");
        break;
     }
   }
   lcd.setCursor(2,0);
   lcd.print("C: #");
   lcd.setCursor(6,0);
   for(int i=0;i<6;i++){
    lcd.print(html2[i]);
   }
 return tsl;
 }

 double leerColor(int outPin, int color, boolean LEDestado){
   estModo(2); // frecuencia max 100Khz => 10us
   //selectiona el filtro
   if(color == 0){
    digitalWrite(S3, LOW); //S3
    digitalWrite(S2, HIGH); //S2
   }else if(color == 1){//rojo
    digitalWrite(S3, LOW); //S3
    digitalWrite(S2, LOW); //S2
   }else if(color == 2){//azul
    digitalWrite(S3, HIGH); //S3
    digitalWrite(S2, LOW); //S2
   }else if(color == 3){//verde
    digitalWrite(S3, HIGH); //S3
    digitalWrite(S2, HIGH); //S2
   }
   if(LEDestado){
    digitalWrite(LT, HIGH);
   }else{
    digitalWrite(LT, LOW);
   }
   //tiempo para que el sensor tome su lectura y se estabilice.
   int delaySensor = 10; //microseg.
   // lee el pulso
   double leerPulso;
   delay(delaySensor);
   leerPulso = pulseIn(outPin, LOW, 25000000);
   //si el tiempo es mayor de lo que lee pulsein regresa cero.
   if(leerPulso < .1){ leerPulso = 25000000; }
     //descativa el sensor para ahorrar energia.
     estModo(0);
     return leerPulso;
 }

 void estModo(int modo){
   if(modo == 0){
    //power OFF
      digitalWrite(LT, LOW);
      digitalWrite(S0, LOW); //S0
      digitalWrite(S1, LOW); //S1
    }else if(modo == 1){
    //escala 1:1
      digitalWrite(S0, HIGH); //S0
      digitalWrite(S1, HIGH); //S1
    }else if(modo == 2){
    //escala 1:5
      digitalWrite(S0, HIGH); //S0
      digitalWrite(S1, LOW); //S1
    }else{ //if(modo == 3)
    //escala 1:50
      digitalWrite(S0, LOW); //S0
      digitalWrite(S1, HIGH); //S1
    }
   }
 
 ///////////////////funcion de configuracion de pines/////////////////
 void TCS3200config(){
  //configuracion de frecuencia en el setup estan los pines respectivos.
  pinMode(S0,OUTPUT);
  pinMode(S1,OUTPUT);
  //seleccion de color.
  pinMode(S2,OUTPUT);
  pinMode(S3,OUTPUT);
  //salida.
  pinMode(out, INPUT);
  //habilita la salida.
  pinMode(OE,OUTPUT); //habilita el modulo de luz.
  // halita la deteccion de color.
  pinMode(LT,OUTPUT);
}
