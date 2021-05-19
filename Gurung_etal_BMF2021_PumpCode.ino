/*
Driving two stepper motors at independent RPMs using Arduino UNO R3 and Adafruit Motorshield (V2)

The one stepper motor is attached to ports M1 and M2 on the Motorshield and the other stepper
motor is connected to ports M3 and M4 on the Motorshield. Arduino UNO R3 is attached the base of
this shield and connected to a computer using a USB 2.0 cable. 

The program prompts the user for input. 
Input Format: 'p speed1 speed2 volume'
where p denotes pump, speed1 and speed2 correspond to desired flow rates (in uL/min),
and volume (in uL) denotes total volume which needs to be pumped (volune is translated into total steps
required for stepper motor) which is same for both motors.

Created 12 September 2020
by Sanaz Agarwal
Email: sanaz.iitd@gmail.com

As part of publication:
Jyoti Gurung, Moein Navvab Kashani, Sanaz Agarwal, Gonzalo Peralta, Murat Gel, and Matthew AB Baker, (2021) "Separation and enrichment of sodium-motile bacteria using cost-effective microfluidics.", Biomicrofluidics.
 */

#include <Wire.h>
#include <Adafruit_MotorShield.h>

Adafruit_MotorShield Motor1(0x60);    

//initialize stepper motors
Adafruit_StepperMotor *Stepper1 = Motor1.getStepper(2048, 1);     
Adafruit_StepperMotor *Stepper2 = Motor1.getStepper(2048, 2);


double vol;                                           //total volume to be pumped
double speed1, speed2;                                //flow rates  
double vol_to_steps=157.696;                          //Default conversion used for our pump for 1ml Syringe
double total_steps1=0, total_steps2=0;
long total_steps;                                     //calculated from total volume

void setup()
{   /* 
    The first function which runs as soon as the program is uploaded on the board.
    It runs only once.                                         */
    
    //initialize serial communication
    Serial.begin(9600);        
    //check_pump();                                         Decomment if the syringe is 5ml instead of 1ml
    Serial.println("Enter the input for motor run!"); 
    Motor1.begin();
    sandbox();             
}

void check_pump(){
  /* Asks user whether the motor is connected to 1ml or 5ml syringe and assigns corresponding
     value of volume to steps conversion factor.
   */
  Serial.println("Enter the size of syringe used used (in ml) -> 1 or 5 ? : ");
  while(Serial.available() == 0){}       //wait for user input
  
  String i = Serial.readString();
  if (i[0] =='5') vol_to_steps=24.02986;
  else if(i[0]=='1')vol_to_steps=157.696;
  else{
    Serial.println("ERROR : Only valid values are '5' or '1'");
    check_pump();
  }                                              
}




void input_string(double &speed1, double &speed2, double &vol) 
{
        
        /* Params: 
             speed1: rate of flow (in uL/min) for pumping of fluid from first syringe. Corresponds to number of steps taken by Stepper motor 1
             speed2: rate of flow (in uL/min) for pumping of fluid from second syringe. Corresponds to number of steps taken by Stepper motor 2
             vol: total volume of fluid which will be pumped by each syringe
           Receive values of flow rates and total volume of fluid. Parse it and store in corresponding 
           assigned parameter variables.                                           */
         
        while(Serial.available()==0){}                     //wait for user input
        String inputString = Serial.readString();          // store user input in string 's'  
        char c = char(inputString[0]);
        
        if(c == 'p' || c == 'P')
        {
            inputString=inputString.substring(2);
            int a1 = inputString.indexOf(' ');
            int a2 = inputString.indexOf(' ', a1+1);
            speed1 = inputString.substring(0, a1).toFloat();
            speed2 = inputString.substring(a1+1,a2).toFloat();
            vol=inputString.substring(a2+1).toFloat();        
        } 
        //Decomment the following code segment to pull/push back the syringe to original position after pumping
        //input 'rewind' for the motor to return to initial position
        /*
        else if (char(s[0]) == 'r' && char(s[1]) =='e' && char(s[2]) == 'w' && char(s[3]) =='i' && char(s[4]) == 'n' && char(s[5]) =='d')
        {
          Serial.println("Return Command Initiated");
          Serial.println(total_steps1);
          Serial.println(total_steps2);
          
          while(total_steps1--){Stepper1->onestep(BACKWARD, DOUBLE);}
          while(total_steps2--){Stepper2->onestep(BACKWARD, DOUBL E); }
          
          Serial.println("Return Command Terminated..............End of Program");
          delay(1500);
          exit(0);
        }
        */

        else {
          Serial.println("Error with input. First character has to be p/P to control input valves or O to control outputvalves");
          input_string(speed1, speed2, vol);
        }
}



void sandbox() {
      /* Calls input_string() function and displays the parameters received from the user
         and then calls runloop() to run the motor.                 */
       
      while (Serial.available() == 0){}           //wait for user input
      //Store parameters given by user
      input_string(speed1, speed2, vol);
      total_steps = vol*vol_to_steps;
      Serial.print("Speed of motor-1 : ");
      Serial.println(speed1);
      Serial.print("Speed of motor-2 : ");
      Serial.println(speed2);
      Serial.print("Total steps taken by motor 1 & 2 : ");
      Serial.println(round(total_steps));   
      Serial.println("Pumping");
      
      runloop();    
      delay(500);
      Serial.println("Enter the input for next motor run :");
}




void loop(){
  /* Runs indefinitely until a condition is met or program is terminated */
   sandbox();      
}



void runloop(){
  /* Converts user input into rotational parameters and loops over the motor steps until the 
     designated number of steps is rotated.             */
   
  double Stepp_1_Interval = 60000000/(vol_to_steps*speed1) ;    //find step interval for motor 1 in microseconds     
  double Stepp_2_Interval = 60000000/(vol_to_steps*speed2);     //find step interval for motor 2 in microseconds  
  long count1=0, count2=0; 
  double current_time=0;
  double previous_time1 = 0, previous_time2 = 0;
  bool a = true;
  
    while (a){
        current_time = micros();  
        if(count1<total_steps){
          if(current_time - previous_time1 > Stepp_1_Interval){
          previous_time1 = current_time;
          Stepper1->onestep(FORWARD, DOUBLE);
          count1++;
          }
        }
        if(count2<total_steps){
          if(current_time - previous_time2 > Stepp_2_Interval){
          previous_time2 = current_time;
          Stepper2->onestep(FORWARD, DOUBLE);
          count2++;
          }
        }
        if((count1==total_steps) && (count2==total_steps)){
          a = false;
          //total_steps1 += count1 ;                     Decomment if motor will be returned to initial position               
          //total_steps2 += count2 ;               
        }
    }
  Serial.println("Pumping Complete!");
  exit;                    //Comment it if you want to input another command after first pumping is complete
}
