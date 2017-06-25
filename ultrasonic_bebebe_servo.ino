//working version
//1. not attack when the bebebe is been mutex
//2. up/down is control servo
//3. BE~~mode is not good. It will bebebe when no thing is in front of my ultrasonic.
// close anti-virs when can't change some file , ex:  rename core/core.a


#include "IRremote.h"
#include <Servo.h>

int receiver = 11; // Signal Pin of IR receiver to Arduino Digital Pin 11
IRrecv irrecv(receiver);           // create instance of 'irrecv'
decode_results results;            // create instance of 'decode_results'

Servo servo_triggor;
int current_angle = 90;


#define TrigPin 2
#define EchoPin 3
int spk=13; //　定義揚聲器為數位接腳13
int DETECT_DISTANCE = 30;
float Value_cm;
bool b_bebebe = true;
int MODE_BEBE = 1; 
#define UPPER_BOUND 179
#define LOWER_BOUND 0
int count_shoot = 0;
int shoot_upper_bound = 50;

int filter_long_be_state = 0;

bool b_power_is_on = true;

void test_shoot_time()
{
  for(int i = 0; i < shoot_upper_bound / 10; i++)
  {
    delay(500);
    be();
  }
}

void shoot()
{
      servo_triggor.write( 170 );      
      delay(300);
      servo_triggor.write( current_angle );
      count_shoot = 0;
}

void add_delta_degree_to_servo(int delta_degree)
{
  int _tmp_current_angle = current_angle + delta_degree;

  if( _tmp_current_angle  > UPPER_BOUND )
  {
    _tmp_current_angle = UPPER_BOUND;    
  }
  else if( _tmp_current_angle < LOWER_BOUND )
  {
    _tmp_current_angle = LOWER_BOUND;    
  }
  
  if( _tmp_current_angle !=  current_angle )
  {
    current_angle = _tmp_current_angle;
    servo_triggor.write( current_angle );
  }   
}

void be()
{
   digitalWrite(spk, 1);
   delay(20);
   digitalWrite(spk, 0);
}
void be_long()
{
   digitalWrite(spk, 1);
   delay(100);
   digitalWrite(spk, 0);
}

void bebebe(float sound_distance)
{ 
  //Serial.print((int)Value_cm);
  //Serial.println("cm");
  if(sound_distance > DETECT_DISTANCE)
  {    
    digitalWrite(spk, 0);   
    count_shoot = 0;
    filter_long_be_state = 0;
    return;
  }
  else
  {
    if(MODE_BEBE % 2 == 1)
    {
        if(b_bebebe)
        {
          digitalWrite(spk, 1);
        }
     
        if(sound_distance > 10)
        {// BE BE BE...
          delay(5 * (int)sound_distance);
          digitalWrite(spk, 0);
        }
    }
    else
    {//BE~~~~~~~~~~
      filter_long_be_state++;
      if( filter_long_be_state > 3 )
      {
        if(b_bebebe)
        {
          digitalWrite(spk, 1);
        }
      }

      count_shoot ++;
      if(count_shoot >= shoot_upper_bound)
      {
        shoot();
      }
    }
    Serial.println(count_shoot);
  }
}

void setup()
{
  Serial.begin(9600);
  pinMode(TrigPin, OUTPUT);
  pinMode(EchoPin, INPUT);
  pinMode(spk, OUTPUT);
  
  Serial.println("IR Receiver Button Decode"); 
  irrecv.enableIRIn(); // Start the receiver

  servo_triggor.attach(9); // attach 9 to servo_triggor
  servo_triggor.write( current_angle );
}

void loop()
{
  if(b_power_is_on)
  {
    digitalWrite(TrigPin, LOW); //低高低电平发一个短时间脉冲去TrigPin
    delayMicroseconds(2);
    digitalWrite(TrigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(TrigPin, LOW);
    Value_cm = float( pulseIn(EchoPin, HIGH) * 17 )/1000; //将回波时间换算成cm
    //接收到的高电平的时间（us）* 340m/s / 2 = 接收到高电平的时间（us） * 17000 cm / 1000000 us = 接收到高电平的时间 * 17 / 1000  (cm)
    bebebe(Value_cm);
  }  
  delay(100);  
  
  if (irrecv.decode(&results)) // have we received an IR signal?
  {
    translateIR(); 
    irrecv.resume(); // receive the next value
  }
}
String hex_current_cmd;
String hex_previous_cmd;

void translateIR() // takes action based on IR code received
// describing Remote IR codes 
{
  Serial.println(results.value, HEX);      
  hex_current_cmd = String(results.value);


  
  if(hex_current_cmd == String(0x514AEB) )
  {
    b_power_is_on = !b_power_is_on;
    be();
  }


  if(b_power_is_on == false)
  {
    return;
  }
  
  if(hex_current_cmd == String(0xFFFFFFFF) )
  {
    hex_current_cmd = hex_previous_cmd;
  }
  
  if(hex_current_cmd == String(0x530ACF) )
  {
    DETECT_DISTANCE += 10;    
    be();
    
  }
  
  if(hex_current_cmd ==  String(0x510AEF) )
  {
    DETECT_DISTANCE -= 10;    
    be();
  }

  //change be mode
  if(hex_current_cmd ==  String(0x511AEE) )
  {
    b_bebebe = true;    
    MODE_BEBE++;    
    be();
  }

  //keyes - shoot mode
  if(hex_current_cmd ==  String(0xFFB04F) )
  {
    b_bebebe = true;    
    MODE_BEBE = 2;    
    be();
  }
  
  //keyes - shoot mode
  if(hex_current_cmd ==  String(0xFF9867) )
  {
    b_bebebe = true;    
    MODE_BEBE = 1;    
    be();
  }
  //keyes - mute mode
  if(hex_current_cmd ==  String(0xFF6897) )
  {
    b_bebebe = false;    
    be();  
  }
  
  //keyes -
  //FFC23D - right


  
  //keyes -
  //FF22DD -left

  if(hex_current_cmd ==  String(0x500AFF) )
  {
    b_bebebe = false;    
    be();
  }
  
  if(hex_current_cmd ==  String(0x524ADB) )
  {
    add_delta_degree_to_servo(10);   
    be();
  }
  
  if(hex_current_cmd ==  String(0x504AFB) )
  {
    add_delta_degree_to_servo(-10);   
    be();
  } 

  if(hex_current_cmd == String(0x518AE7) )
  {
    shoot_upper_bound = shoot_upper_bound + 10;   
    be();
  }

  if(hex_current_cmd == String(0x538AC7) )
  {
    if( shoot_upper_bound - 10 > 0)
    {
      shoot_upper_bound = shoot_upper_bound - 10;   
      be();
    }
    else
    {
      shoot_upper_bound = 1;
      be();
      delay(50);
      be();
      delay(50);
      be();
    }    
  }
  if(hex_current_cmd == String(0x50EAF1) )
  {
    be_long();
    test_shoot_time();
  }
  //Serial.println(servo.read(), DEC);
 /*
  */
  hex_previous_cmd = hex_current_cmd;

 
/*
  switch(results.value)

  {

  case 0xFF629D: Serial.println(" FORWARD"); break;
  case 0xFF22DD: Serial.println(" LEFT");    break;
  case 0xFF02FD: Serial.println(" -OK-");    break;
  case 0xFFC23D: Serial.println(" RIGHT");   break;
  case 0xFFA857: Serial.println(" REVERSE"); break;
  case 0xFF6897: Serial.println(" 1");    break;
  case 0xFF9867: Serial.println(" 2");    break;
  case 0xFFB04F: Serial.println(" 3");    break;
  case 0xFF30CF: Serial.println(" 4");    break;
  case 0xFF18E7: Serial.println(" 5");    break;
  case 0xFF7A85: Serial.println(" 6");    break;
  case 0xFF10EF: Serial.println(" 7");    break;
  case 0xFF38C7: Serial.println(" 8");    break;
  case 0xFF5AA5: Serial.println(" 9");    break;
  case 0xFF42BD: Serial.println(" *");    break;
  case 0xFF4AB5: Serial.println(" 0");    break;
  case 0xFF52AD: Serial.println(" #");    break;
  case 0xFFFFFFFF: Serial.println(" REPEAT");break;  

  default:      
   //Serial.println(results.value, HEX);
     Serial.println(" other");
  }// End Case
  */
  delay(50); // Do not get immediate repeat
} //END translateIR
