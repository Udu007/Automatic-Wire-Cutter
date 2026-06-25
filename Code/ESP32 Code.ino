// Global variables
float Kp=1.0,Ki=0.05,Kd=0.01;
float target_pos=0;
float current_pos=0;
float last_time=0;

void setup()
{
  last_time=millis();
  Serial.begin(9600);
}
x
void loop()
{
  // Input from UART
  if ( Serial.available()>0)
  {
    target_pos=Serial.parseFloat();
    current_pos=Serial.parseFloat();
  }

  float output_vel=computePID(target_pos,current_pos);

  Serial.println(output_vel);
  delay(50);
}

float computePID( float target, float current)
{
  float current_time=millis();
  float dt=(current_time-last_time)/1000.0;

  // Write Your Code Here

  last_time=current_time;

  return 0.0; // Return your output
}