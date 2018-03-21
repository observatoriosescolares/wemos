boolean encendido = false; //Variable global
boolean boton_pulsado=false;
void setup() {
  // put your setup code here, to run once:
  pinMode(14, OUTPUT); //D5
  pinMode(15, INPUT);  //D8 con pull down  
}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(15) == HIGH && boton_pulsado==false) { 
    boton_pulsado=true;   
    encendido = !encendido;
    if (encendido == true) {
      digitalWrite(14, HIGH);
    } else {
      digitalWrite(14, LOW);
    }  
    delay(100);// Demora anti-rebotes bajada 
  }
  if (digitalRead(15) == LOW){
    boton_pulsado=false;
    delay(100);// Demora anti-rebotes subida
  }
}
