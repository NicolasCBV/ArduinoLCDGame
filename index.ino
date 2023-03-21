#include <EEPROM.h>

#include <LiquidCrystal.h>
  
bool game, vFire, vEnergy, vCoin=false;
double decreaseEnergy=0.25;
double totalEnergy=100;

uint16_t points;

/*========time variables=========*/
long double clockTime;
double vel=122;
long batteryTime, coinTime;
#define longTime 1000
#define shortTime 950
long initialTime;

/*==========POSITION VARIABLES====*/
short int PXshoot;
short int PYnave;
short int PXcoin;
short int PYcoin;
short int PXaste;
short int PXbattery;

/*define buttons*/
#define btFire 5
#define btUp 16
#define btDown 17

bool button[3];

LiquidCrystal lcd(6, 7, 8, 9, 14, 15); //RS, Enable, D4, D5, D6, D7

/*Create figures*/
byte nave[8]={B00111, B01100, B11110, B01011, B01011, B11110, B01100, B00111};
byte asteroide[8]={B00000, B00100, B01110, B10111, B11101, B01110, B00100, B00000};
byte explosao[8]={B10001, B10101, B01010, B10100, B00101, B01010, B10101, B10001};
byte energia[8]={B01110, B11011, B10001, B10101, B10101, B10101, B10001, B11111};
byte tiro[8]={B00000, B00000, B00011, B00011, B00000, B00000, B00000, B00000};
byte naveInimiga[8]={B00100,B00100,B01101,B11110,B11110,B01101,B00100,B00100};
byte satelite[8]={B00111,B00010,B01110,B11011,B11011,B01110,B00010,B00111};
byte drone[8]={B10110,B01011,B01111,B11100,B11100,B01111,B01011,B10110};

/*functions*/
/*=======LAYOUT==========*/
void window(bool cond);
void statusGame();

/*=======DECISIONS=======*/
void checkHudButtons(bool bt, short int i);
void action(short int button);
void moveElement(short &PX, byte icon);

/*=======ENEMIE MOVIMENTATION======*/
void enemieMove(short int &PXaste);

/*=======DRAW ITEMS================*/
void drawObjt(int PX, int PY, int num);
void drawObjt(int PX, int PY, int num);
void drawChar(int py, int px, char letter);

/*=========score records===========*/
void scoreRecords(unsigned int points);

/*==========RESET VARIABLES=========*/
void reset();


class objetos{
private:
    int pX, pY;
    long tempo;
    int hp;
public:
    objetos(int PX, bool PY, int Hp=0);

    long getDocumentation(int num){
        switch(num){
            case 1:
                return pX;
            break;

            case 2:
                return pY;
            break;

            case 3:
                return tempo;
            break;

            case 4:
                return hp;
            break;
        }
    }
    
    void setDocumentationIncreased(short *PX){
      *PX = *PX+1;
      pX=*PX;
      }

     void setDocumentationDecreased(short int *PX){
      *PX = *PX-1;
      pX=*PX;
      }
      
    void setPY(int py){
        pY=py;
    }
    void setPX(int px){
      pX=px;
      }
    void setPositions(int px, int py){
      pY=py;
      pX=px;
      }
};  

objetos::objetos(int PX, bool PY, int Hp){
    pX=PX;
    pY=PY;
    hp=Hp;
}

//Organize class objects
objetos Player(0, 0, 4);
objetos Aste(14, random(0,2));
objetos PlayerShoot(-1,0);
objetos Battery(13,0);
objetos Moeda(13,1);
    
void setup() {
  /*========for the first inicialization===*/
  if(EEPROM.read(2)>2)
    EEPROM.write(2, 0);
  //////////OBJECTS/////////
  lcd.createChar(1, nave);
  lcd.createChar(2, asteroide);
  lcd.createChar(3, tiro);
  lcd.createChar(4, explosao);
  lcd.createChar(5, energia);
  lcd.createChar(6, naveInimiga);
  lcd.createChar(7, satelite);
  lcd.begin(16,2);
  while(digitalRead(btFire)==0){
    window(0);
    delay(250);
   }
  
///////////////////////////////// 
}

void loop() {
  
   delay(180);
   while(true){
    /*===========objects locations=============*/
     PXshoot = PlayerShoot.getDocumentation(1);
     PYnave = Player.getDocumentation(2);
     PXaste = Aste.getDocumentation(1);
     PXbattery = Battery.getDocumentation(1);
     PXcoin = Moeda.getDocumentation(1);
     PYcoin = Moeda.getDocumentation(2);
     
    /*=====================hud======================*/
     button[0] = digitalRead(btDown);
     button[1] = digitalRead(btUp);
     button[2] = digitalRead(btFire);
     
     /*=================Lambdas======================*/
     /*=================vel=====================*/
     auto velReduce = [](){
      vel = vel - (vel*0.1)/100;
     };
    
      auto velIncrease = [](){
         vel = vel - (vel*0.1)/100;
      };
     /*=================energy==================*/
     auto addEnergy = [](){
      Battery.setPX(13);
      totalEnergy = totalEnergy + 40;
      vEnergy = false;
      };
  
      auto spawnBattery = [](){
        if((random(0,60)>58) && (totalEnergy < 50)){
           vEnergy=true;
           Battery.setPX(13);
           Battery.setPY(random(0,2));
        }
      };
     auto batterys = [spawnBattery](){
      if(millis() >= batteryTime+275){
        batteryTime=millis();
        (vEnergy) ? moveElement(PXbattery, 5) : spawnBattery();
      }
      (vEnergy) ? drawObjt(Battery.getDocumentation(1), Battery.getDocumentation(2), 5) : spawnBattery();
     };
     /*=================coins======================*/

      auto addPoints = [](){
        Moeda.setPX(13);
        points = points + 5;
        vCoin = false;
      };
  
      auto spawnCoin = [](){
        if(random(0,60)>58){
           vCoin=true;
           Moeda.setPX(13);
           Moeda.setPY(random(0,2));
        }
      };
      
     auto coins = [spawnCoin](){
      if(millis() >= coinTime+375){
        coinTime=millis();
        (vCoin) ? moveElement(PXcoin, 1) : spawnCoin();
      }
      (vCoin) ? drawChar(Moeda.getDocumentation(1), Moeda.getDocumentation(2), '$') : spawnCoin();
     };
     
     /*=================Bullets======================*/
     auto resetPositionBullet = [](){
        PlayerShoot.setPX(-1);
        vFire=false;   
      };
  
    auto moveBullet = [](){
      if(vFire){
         drawObjt(0, Player.getDocumentation(2), 1);
         moveElement(PXshoot, 3);
      }
        
    };
    /*=======spawn aste=======*/
     auto spawnAste = [](short PXaste){
       drawObjt(0, Player.getDocumentation(2), 1);
       Aste.setPX(13);
       enemieMove(PXaste);
     };
  
     auto explodeEnemieAste = [](){
        PlayerShoot.setPX(-1);
        vFire=false;   
  
        points++;
        
        drawObjt(Aste.getDocumentation(1), Aste.getDocumentation(2), 4);
        delay(100);   
        
        Aste.setPX(13);
        Aste.setPY(random(0,2));
      };
      /*=========player lose==========*/
      auto lose = [](){
        lcd.clear();
        
        drawObjt(Player.getDocumentation(1), Player.getDocumentation(2), 4);
  
        if((((EEPROM.read(0) *256)+ EEPROM.read(1)) < points) || (EEPROM.read(2)==0)){
          EEPROM.write(0, points / 256);
          EEPROM.write(1, points % 256);
          EEPROM.write(2, 1);
        }
        
        delay(580);
  
        
  
        lcd.clear();
        
        window(1);
        
        while(digitalRead(btFire)==0){
          reset();
          }
        delay(200);
        
        };
    /////////////////////////////
  
     drawObjt(Player.getDocumentation(1), Player.getDocumentation(2), 1);
     statusGame(13);
               
            if(millis()>=clockTime+vel){
               clockTime=millis();
        
               totalEnergy = totalEnergy-decreaseEnergy;
               
               lcd.clear();   //refresh layout
  
              /*===draw aste===*/
               drawObjt(Aste.getDocumentation(1), Aste.getDocumentation(2), 2);          
               
  
               
                           /*=====Events====*/
                /*===========shoots and enemies====================*/
               ((PXshoot<13)&&(vFire)) ?  moveBullet() : resetPositionBullet();
  
              
               (Aste.getDocumentation(1)<0) ? spawnAste(PXaste) :
               
                (((PlayerShoot.getDocumentation(1))==(Aste.getDocumentation(1)))  &&
                ((PlayerShoot.getDocumentation(2))==(Aste.getDocumentation(2))))  
                                              ||
                (((PlayerShoot.getDocumentation(1))==(Aste.getDocumentation(1)+1))  &&
                ((PlayerShoot.getDocumentation(2))==(Aste.getDocumentation(2)))) ? explodeEnemieAste() :
                
                (((Player.getDocumentation(1))==(Aste.getDocumentation(1)))  &&
                ((Player.getDocumentation(2))==(Aste.getDocumentation(2))))
                                              ||
                                       (totalEnergy<=0)? lose() 
                
                                               : 
                                               
                (((Player.getDocumentation(1))==(Battery.getDocumentation(1)))  &&
                ((Player.getDocumentation(2))==(Battery.getDocumentation(2))))  
                                              ||
                (((Player.getDocumentation(1))==(Battery.getDocumentation(1)+1))  &&
                ((Player.getDocumentation(2))==(Battery.getDocumentation(2)))) ?
                 addEnergy()                 
                                               : 

                (((Player.getDocumentation(1))==(Moeda.getDocumentation(1)))  &&
                ((Player.getDocumentation(2))==(Moeda.getDocumentation(2))))  
                                              ||
                (((Player.getDocumentation(1))==(Moeda.getDocumentation(1)+1))  &&
                ((Player.getDocumentation(2))==(Moeda.getDocumentation(2)))) ?
                addPoints() : enemieMove(PXaste);
  
                /*=================consumables============================*/
                  batterys();
                  coins();
  
                if(PXbattery<0){
                  PXbattery = 13;
                  vEnergy = false;
                }

                if(PXcoin<0){
                  PXcoin = 13;
                  vCoin = false;
                }
  
                if(vel>=60)
                 velReduce();
                 else
                  vel = 122;
                 
                 
                /*=============check hud=============*/
                checkHudButtons(button[2], 2);
            }
                checkHudButtons(button[0], 0);
                checkHudButtons(button[1], 1);
   }

}


/*================LAYOUT==============================*/
void window(bool cond){ //cond 0 = home || cond 1 = endgame
    switch(cond){
      case 0:   //home
        lcd.setCursor(3,0);
        lcd.print("NC SHOOT");
        lcd.setCursor(0,1);
        lcd.print(" Pressione TIRO");

        break;
        
        case 1:   //end game
          lcd.clear();
          lcd.print(" RECORDE:");
          lcd.setCursor(10,0);
          lcd.print( (EEPROM.read(0) * 256)+EEPROM.read(1) );
          lcd.print("pt");
          lcd.setCursor(0,1);
          lcd.print(" Pressione TIRO");
        
        break;

     }
}
void statusGame(short int PX){
  lcd.setCursor(PX, 0);
  lcd.print(points);
  lcd.setCursor(PX, 1);
  lcd.print(totalEnergy);  
}
/*================hud buttons and actions=============*/
 void checkHudButtons(bool bt, short i){
         switch (bt){
           case 0:
           break;
  
           case 1:
            action(i);
           break;
          }
 }

 void action(short button){
    switch (button){
      case 0:
        Player.setPY(1); 
      break;
      
      case 1:
        Player.setPY(0);
      break;
      
      case 2:
       PlayerShoot.setPY(Player.getDocumentation(2));
       vFire=true;
      break;
    }
  }
void moveElement(short &PX, byte icon){
  switch (icon){
    case 5:
      Battery.setDocumentationDecreased(&PX);
      drawObjt(Battery.getDocumentation(1), Battery.getDocumentation(2), icon);
    break;
     
    case 3:
      PlayerShoot.setDocumentationIncreased(&PX);
      drawObjt(PlayerShoot.getDocumentation(1), PlayerShoot.getDocumentation(2), 3);
    break;

    case 1:
      Moeda.setDocumentationDecreased(&PX);
      drawChar(Moeda.getDocumentation(1), Moeda.getDocumentation(2), '$');
     break;
  }
  }
/*===============enemies move====================*/
void enemieMove(short &PXaste){
  Aste.setDocumentationDecreased(&PXaste);
  if(Aste.getDocumentation(1)<0){
    Aste.setPY(random(0,2));
    Aste.setPX(14);
   }
}

/*==============draw items===============*/
void drawObjt(int PX, int PY, int num){
        lcd.setCursor(PX,PY);
        lcd.write(num);
    }

void drawChar(int px, int py, char letter){
  lcd.setCursor(px, py);
  lcd.print(letter);
  }

/*==============reset items===============*/
void reset(){
  Player.setPositions(0,0);
  Aste.setPositions(13, random(0,2));
  PlayerShoot.setPositions(-1,0);
  Battery.setPositions(13,0);
  Moeda.setPositions(13,1);
  
  game=true;
  clockTime=0;
  vel=122;
  
  vFire, vEnergy=false;
  decreaseEnergy=0.25;
  totalEnergy=100;
  points=0;
  }