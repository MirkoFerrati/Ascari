int M2 = 4;
int E2 = 5; 
int E1 = 6;
int M1 = 7;
int a=0;
 
void setup() 
{ 
    pinMode(M1, OUTPUT);   
    pinMode(M2, OUTPUT); 
    pinMode(13, OUTPUT);
} 
 
void loop() 
{ 
    digitalWrite(13,HIGH);
    
    if(a==0)
    {
        digitalWrite(M1,HIGH);   
        digitalWrite(M2,HIGH);       
        analogWrite(E1, 50);
        analogWrite(E2, 255);
        delay(20000); 

        digitalWrite(M1,LOW);   
        digitalWrite(M2,LOW); 
        analogWrite(E1, 0);
        analogWrite(E2, 0);
        
        a=1;
    }
    else
    {
         delay(500);
         digitalWrite(13,LOW);
         delay(500); 
    }
}
