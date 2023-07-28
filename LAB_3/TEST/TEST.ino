int list[] ={23, 22, 24, 25, 47, 2, 52, 51, 50, 49, 53};
void setup(){
    for (int i = 0; i < 11; i++){
        pinMode(list[i], OUTPUT);
    }
    
}

void loop(){

    digitalWrite(23, LOW);
    digitalWrite(22, HIGH);
    digitalWrite(24, HIGH);
    digitalWrite(25, HIGH);

    digitalWrite(53, HIGH);
    digitalWrite(49, HIGH);
    digitalWrite(52, HIGH);

    //===== DELAY =====


    digitalWrite(23, HIGH);
    digitalWrite(22, LOW);
    digitalWrite(24, HIGH);
    digitalWrite(25, HIGH);

    digitalWrite(53, LOW);
    digitalWrite(49, LOW);
    digitalWrite(52, LOW);

    digitalWrite(47, HIGH);
    digitalWrite(51, HIGH);

    //===== DELAY =====

    digitalWrite(23, HIGH);
    digitalWrite(22, HIGH);
    digitalWrite(24, LOW);
    digitalWrite(25, HIGH);


    digitalWrite(47, HIGH);
    digitalWrite(51, HIGH);

    //===== DELAY =====

    digitalWrite(23, HIGH);
    digitalWrite(22, HIGH);
    digitalWrite(24, HIGH);
    digitalWrite(25, LOW);

    digitalWrite(47, LOW);
    digitalWrite(51, LOW);
    
    digitalWrite(2, HIGH);
    digitalWrite(53, HIGH);
    digitalWrite(50, HIGH);

     //===== DELAY =====

    digitalWrite(2, LOW);
    digitalWrite(53, LOW);
    digitalWrite(50, LOW);
}