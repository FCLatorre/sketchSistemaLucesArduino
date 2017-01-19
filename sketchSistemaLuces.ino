int posicion = 13;
int cruce = 12;
int carretera = 11;
String estado="OFF";
boolean encendidasAUTO = false;
//Configurable quizás
int contador = 0;

//Configurables
int umbralSuperior = 600;
int umbralInferior = 400;
int tiempoRafagas = 2000;



void setup() {
  // put your setup code here, to run once:
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(A0, INPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.setTimeout(100);
  if(Serial.available() > 0){
    String input = Serial.readString();
    if(input.startsWith("DATA:")){
      int aux = input.indexOf('%');
      int aux2 = input.indexOf('%', aux+1);
      int aux3 = input.indexOf('%', aux2+1);
      String nuevoUmbralInferior = input.substring(aux+1,aux2);
      String nuevoUmbralSuperior = input.substring(aux2+1, aux3);
      String nuevoTiempoRafagas = input.substring(aux3+1);
      umbralInferior = nuevoUmbralInferior.toInt();
      umbralSuperior = nuevoUmbralSuperior.toInt();
      tiempoRafagas = nuevoTiempoRafagas.toInt()*1000;
    } 
    else {
        if(input == "OFF" && estado!=input){
          digitalWrite(posicion, LOW);
          digitalWrite(cruce, LOW);
          digitalWrite(carretera, LOW);
        } 
        else if(input=="AUTO" && estado!=input) {
          digitalWrite(posicion, LOW);
        }
        else if(input=="POSICION" && estado!=input) {
          digitalWrite(posicion, HIGH);
          digitalWrite(cruce, LOW);
          digitalWrite(carretera, LOW);
        }
        else if(input=="CRUCE" && estado!=input) {
          digitalWrite(posicion, HIGH);
          digitalWrite(cruce, HIGH);
          digitalWrite(carretera, LOW);
        }
        else if(input=="CARRETERA" && estado!=input) {
          digitalWrite(posicion, LOW);
          digitalWrite(cruce, LOW);
          digitalWrite(carretera, HIGH);
        }
        else if(input=="RAFAGA" && estado!=input && estado!="CARRETERA") {
          //Se dejan el resto de luces encendidas como estén porque las largas se sobreponen al resto
          digitalWrite(carretera, HIGH);
          delay(tiempoRafagas);
          digitalWrite(carretera, LOW);
        }
    
        if(estado!=input && input!="RAFAGA"){
          Serial.print("MSG:Cambio de modo.%");
          Serial.print(estado);
          Serial.print("%");
          Serial.println(input);
          estado = input;
        }
      }
    
   }

   //Leer el valor del sensor de luz
   int nivelLuzAmbiente = analogRead(A0);

   
   //Mandar el valor leido a la gráfica
   if(contador==5000){
     Serial.print("STAT: %");
     Serial.print(nivelLuzAmbiente);
     Serial.print("%");
     Serial.print(umbralSuperior);
     Serial.print("%");
     Serial.println(umbralInferior);
     contador=0;
   } else {
    contador++; 
   }

    //Si el valor es mayor que el umbral>superior yy estamos en automático
    if(nivelLuzAmbiente > umbralSuperior && estado=="AUTO"){
      // Apagamos cruce y posición y mandamos evento a la plataforma
      digitalWrite(posicion, LOW);
      digitalWrite(cruce, LOW);
      if(encendidasAUTO==true){
        Serial.println("MSG:Valor detectado mayor que umbral superior. Apagar Luces");
        encendidasAUTO=false;
      }
    }

    //Si el valor es menor que el umbral inferior yy estamos en automático
    if(nivelLuzAmbiente < umbralInferior && estado=="AUTO"){
      //Encendemos posicion y cruce y mandamos evento a la plataforma
      digitalWrite(posicion, HIGH);
      digitalWrite(cruce, HIGH);
      if(encendidasAUTO==false){
        Serial.println("MSG:Valor detectado menor que umbral inferior. Encender Luces");
        encendidasAUTO=true;
      }  
    }   
}
