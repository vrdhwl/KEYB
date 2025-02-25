#include <Keyboard.h>                           
#include <KeyboardBLE.h>
int longPressDelay = 100;                   //customizable encoderValues
int spamSpeed = 15;
int rowval=5;
int colval=14;
char KEY_SPACE = ' ';
byte inputs[] = {6, 2, 8, 9, 10, 11, 12, 16,17,18,19,20,21,22};          //declaring inputs and outputs
const int inCount = sizeof(inputs)/sizeof(inputs[0]);
byte outputs[] = {0,1,7,3,4};
const int outCount = sizeof(outputs)/sizeof(outputs[0]);

char layout[5][14] = {               //layout grid for characters
    {KEY_ESC,   '1',   '2',   '3',   '4',   '5',   '6',   '7',   '8',   '9',   '0', '-',  '=',  KEY_BACKSPACE },
    {KEY_TAB,    ' ',    'w',    'e',    'r',    't',    'y',    'u',    'i',    'o',    'p', '[', ']', '\\' },
    {KEY_CAPS_LOCK,    'a',    's',    'd',    'q',    'g',    'h',    'j',    'k',    'l', ';', '\'', ' ',    KEY_RETURN },
    {KEY_LEFT_SHIFT,    'z',    'x',    'c',    'v',    'b',    'n',    'm', ',',  '.', '/', '_', ' ',    KEY_UP_ARROW },
    {KEY_LEFT_CTRL, KEY_LEFT_GUI, KEY_LEFT_ALT,' ',  'f',' ', ' ', ' ', ' ',  ' ', KEY_PRINT_SCREEN, KEY_LEFT_ARROW, KEY_DOWN_ARROW, KEY_RIGHT_ARROW }
};



int keyDown[5][14];
bool keyLong[5][14];


void setup(){
  
  for(int i=0; i<outCount; i++){    //declaring all the outputs and setting them high
    pinMode(outputs[i],OUTPUT);
    digitalWrite(outputs[i],HIGH);
  }
  for(int i=0; i<inCount; i++){     //declaring all the inputs and activating the internal pullup resistor
    pinMode(inputs[i],INPUT_PULLUP);
  }
  
 Serial.begin(115200);
  KeyboardBLE.begin();
  delay(5000);

}

//Main loop going through all the keys, then waiting 0.5ms
void loop() {
  for (int i=0; i<rowval; i++)
  {    
    digitalWrite(outputs[i],LOW);   //setting one row low
    delayMicroseconds(5);           //giving electronics time to settle down
    
    for(int j=0; j<colval; j++)
    {
      if(digitalRead(inputs[j]) == LOW)
      {
        keyPressed(i,j);            //calling keyPressed function if one of the inputs reads low
      }
      else if(keyDown[i][j] != 0)   //resetting the key if it is not pressed any more
      {  
        resetKey(i,j);
      }
    }
    
    digitalWrite(outputs[i],HIGH);
    delayMicroseconds(500);         //setting the row high and waiting 0.5ms until next cycle
  }

}

//if a key is pressed, this Funtion is called and outputs to serial the key location, it also sends the keystroke if not already done so
void keyPressed(int row, int col){
  Serial.print("Output: "); 
  Serial.print(row);
  Serial.print(" Input: ");
  Serial.print(col);
  Serial.print(" ");
  Serial.println(layout[row][col]);
 
   if (layout[row][col] == KEY_LEFT_SHIFT  )
   { KeyboardBLE.press(layout[row][col]);
   }

   if (layout[row][col] == KEY_LEFT_CTRL )
   { KeyboardBLE.press(layout[row][col]);
   }

   if (layout[row][col] == KEY_LEFT_ALT )
   { KeyboardBLE.press(layout[row][col]);
   }

        
   if(keyDown[row][col]==0){   //if the function is called for the first time for this key
    if (layout[row][col]== KEY_LEFT_GUI)
    { KeyboardBLE.press(layout[row][col]);
    }
    else {
    KeyboardBLE.write(layout[row][col]);
    }
  }
  else if(keyLong[row][col] && keyDown[row][col] > spamSpeed){//if the key has been held long enough to warrant another keystroke set
    KeyboardBLE.press(layout[row][col]);
    keyDown[row][col] = 1;
  }
  else if(keyDown[row][col] > longPressDelay){ //if the key has been held for longer that longPressDelay, it switches into spam mode
    keyLong[row][col] = true;
  }

  keyDown[row][col]++;
}

void resetKey(int row, int col){ //resetting the variables after key is released        
  keyDown[row][col] = 0;
  keyLong[row][col] = false;
  KeyboardBLE.release(layout[row][col]);
}
