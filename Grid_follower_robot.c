#include "main.h"


#define Left_2_PORT  GPIOA
#define Left_2       GPIO_PIN_9
#define Left_1_PORT  GPIOA
#define Left_1       GPIO_PIN_8
#define Right_4_PORT GPIOB
#define Right_4      GPIO_PIN_10
#define Right_3_PORT GPIOB
#define Right_3      GPIO_PIN_11
#define Right_Sensor_PORT GPIOA
#define Right_Sensor_PIN  GPIO_PIN_15
#define Left_Sensor_PORT  GPIOA
#define Left_Sensor_PIN   GPIO_PIN_4
#define TRIG_PIN GPIO_PIN_13
#define TRIG_PORT GPIOB
#define ECHO_PIN GPIO_PIN_12
#define ECHO_PORT GPIOB
uint32_t pMillis;
uint32_t Value1 = 0;
uint32_t Value2 = 0;
uint16_t Distance  = 0;  // cm


TIM_HandleTypeDef htim1;

ADC_HandleTypeDef hadc1;


TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;


void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM1_Init(void);


int main(void)
{

  HAL_Init();


  SystemClock_Config();

  MX_GPIO_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  HAL_ADC_Start(&hadc1);
  /* USER CODE END 2 */
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
  TIM3->CCR1 = 0;
  TIM4->CCR1 = 0;
  int wait = 1000;
  int duration = 5000;
  int time = 4000;
  int R_EYE, L_EYE;


  int t_1 = 10; //5
  int t_2 = 30; //25
  int Speed_H = 170;
  int Speed_M = 150;
  int Speed_L = 130;
  char Center = 'D';
  char Target;
  char current_state;
  char next_state;
  char New_Loc;

  // Timer 3 = right && Timer 4 = Left
  int data;
  int button;
  int pulse = 0;
  int Long = 71;
  int Short = 41;
  int meter = 101;
    /*
     * A < 1100
     * 1500 < B < 2200
     * 2800 < C < 3400
     * 3800 < D
     *
     *  */

  HAL_TIM_Base_Start(&htim1);
  HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);  // pull the TRIG pin low
  /* USER CODE END 2 */
void GetObjects ()
{
    HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_SET);  // pull the TRIG pin HIGH
    __HAL_TIM_SET_COUNTER(&htim1, 0);
    while (__HAL_TIM_GET_COUNTER (&htim1) < 10);  // wait for 10 us
    HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);  // pull the TRIG pin low

    pMillis = HAL_GetTick(); // used this to avoid infinite while loop  (for timeout)
    // wait for the echo pin to go high
    while (!(HAL_GPIO_ReadPin (ECHO_PORT, ECHO_PIN)) && pMillis + 10 >  HAL_GetTick());
    Value1 = __HAL_TIM_GET_COUNTER (&htim1);

    pMillis = HAL_GetTick(); // used this to avoid infinite while loop (for timeout)
    // wait for the echo pin to go low
    while ((HAL_GPIO_ReadPin (ECHO_PORT, ECHO_PIN)) && pMillis + 50 > HAL_GetTick());
    Value2 = __HAL_TIM_GET_COUNTER (&htim1);

    Distance = (Value2-Value1)* 0.034/2;
}

void info ()
{
	 R_EYE = HAL_GPIO_ReadPin(Right_Sensor_PORT, Right_Sensor_PIN);
	 L_EYE = HAL_GPIO_ReadPin(Left_Sensor_PORT, Left_Sensor_PIN);
}

void Walk ()
{
     //Left Forward
     HAL_GPIO_WritePin(Left_2_PORT, Left_2, 0);
     HAL_GPIO_WritePin(Left_1_PORT, Left_1, 1);
     //Right Forward
     HAL_GPIO_WritePin(Right_4_PORT, Right_4, 0);
     HAL_GPIO_WritePin(Right_3_PORT, Right_3, 1);
     HAL_Delay(t_1);
     info ();
     //Stop
     HAL_GPIO_WritePin(Left_2_PORT, Left_2, 0);
     HAL_GPIO_WritePin(Left_1_PORT, Left_1, 0);
     HAL_GPIO_WritePin(Right_4_PORT, Right_4, 0);
     HAL_GPIO_WritePin(Right_3_PORT, Right_3, 0);
     HAL_Delay(t_2);
}
void Line_Forward()
{
	 info ();
	 while(R_EYE != 1 || L_EYE != 1)
	 {
		 if (R_EYE == 0 && L_EYE == 0)
		 	 {
		      //forward
			  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 1);
		      TIM3->CCR1 = Speed_M;
		      TIM4->CCR1 = Speed_M;
              Walk();
		 	  info ();
		 	      }
		 else if (R_EYE == 1 && L_EYE == 0)
		 	     {
		 	      //forward and turn left
		 	      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 1);
		 	      TIM3->CCR1 = Speed_L;
		 	      TIM4->CCR1 = Speed_H;
                  Walk();
		 	      info ();
		 	      	 }
		 else if (L_EYE == 1 && R_EYE == 0)
		 	     {
		 	      //forward and turn right
		 	      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 1);
		 	      TIM3->CCR1 = Speed_H;
		 	      TIM4->CCR1 = Speed_L;
                  Walk();
		 	      info ();
		 	      	  }

	 }
	 HAL_Delay(wait);

}

void Take_Off()
{
      //forward
	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 1);
	  TIM3->CCR1 = Speed_M;
	  TIM4->CCR1 = Speed_M;
      info();
	  while (R_EYE == 1 && L_EYE == 1)
	  {
      Walk();
	  info();
	  }
	  for (int i =0; i < 10; i++)//20
	  {
       Walk();
	  }
	  HAL_Delay(wait);
}

void Take_Forward()
{
	Take_Off();
	Line_Forward();
}

void SelftoLeft()
{
    //Left Backward
    HAL_GPIO_WritePin(Left_2_PORT, Left_2, 1);
    HAL_GPIO_WritePin(Left_1_PORT, Left_1, 0);
    //Right Forward
	HAL_GPIO_WritePin(Right_4_PORT, Right_4, 0);
	HAL_GPIO_WritePin(Right_3_PORT, Right_3, 1);
	HAL_Delay(t_1);
	//Stop
    HAL_GPIO_WritePin(Left_2_PORT, Left_2, 0);
    HAL_GPIO_WritePin(Left_1_PORT, Left_1, 0);
    HAL_GPIO_WritePin(Right_4_PORT, Right_4, 0);
    HAL_GPIO_WritePin(Right_3_PORT, Right_3, 0);
	HAL_Delay(t_2);
}

void Turn_Left()
{
	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 1);
	  TIM3->CCR1 = Speed_H;
	  TIM4->CCR1 = Speed_H;
	  info();
	  if (L_EYE == 1)
	  {
		  while (L_EYE != 0)
		  {
		  SelftoLeft();
		  info();
		  }
	  }
	  while (L_EYE != 1)
	  {
      SelftoLeft();
	  info();
	  }
	  while (L_EYE != 0)
	  {
	  SelftoLeft();
	  info();
	  }
	  /*while (R_EYE != 1)
	  {
	  SelftoLeft();
	  info();
	  	  }*/
	  HAL_Delay(wait);
}

void Left_Forward()
{
	Take_Off();
	Turn_Left();
	Line_Forward();
}

void SelftoRight()
{
	  //Left Forward
	  HAL_GPIO_WritePin(Left_2_PORT, Left_2, 0);
	  HAL_GPIO_WritePin(Left_1_PORT, Left_1, 1);
	  //Right Backward
	  HAL_GPIO_WritePin(Right_4_PORT, Right_4, 1);
	  HAL_GPIO_WritePin(Right_3_PORT, Right_3, 0);
	  HAL_Delay(t_1);
	  info();
	  //Stop
	  HAL_GPIO_WritePin(Left_2_PORT, Left_2, 0);
	  HAL_GPIO_WritePin(Left_1_PORT, Left_1, 0);
	  HAL_GPIO_WritePin(Right_4_PORT, Right_4, 0);
	  HAL_GPIO_WritePin(Right_3_PORT, Right_3, 0);
	  HAL_Delay(t_2);
}

void Turn_Right()
{
	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 1);
	  TIM3->CCR1 = Speed_H;
	  TIM4->CCR1 = Speed_H;
	  info();
	  if (R_EYE == 1)
	  {
		  while (R_EYE != 0)
		  {
		  SelftoRight();
		  info();
		  }
	  }
	  while (R_EYE != 1)
	  {
	  SelftoRight();
	  info();
	  	  }
	  while (R_EYE != 0)
	  {
	  SelftoRight();
	  info();
	  }
	  /*while (L_EYE != 1)
	  {
	  SelftoRight();
	  info();
	  }*/
	  HAL_Delay(wait);
}

void Right_Forward()
{
	Take_Off();
	Turn_Right();
	Line_Forward();
}

void Stop()
{
     HAL_GPIO_WritePin(Left_2_PORT, Left_2, 0);
     HAL_GPIO_WritePin(Left_1_PORT, Left_1, 0);
     HAL_GPIO_WritePin(Right_4_PORT, Right_4, 0);
     HAL_GPIO_WritePin(Right_3_PORT, Right_3, 0);
     HAL_Delay(wait);
}

void Turning_180()
{
	      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 1);
		  TIM3->CCR1 = Speed_M;
		  TIM4->CCR1 = Speed_M;
		  while (L_EYE != 0)
		  {
          SelftoLeft();
		  info();
		  }
		  while (R_EYE != 0)
		  {
		  SelftoLeft();
		  info();
		  }
		  while (L_EYE != 1)
		  {
		  SelftoLeft();
		  info();
		  }
		  while (R_EYE != 0 || L_EYE != 0)
		  {
		  SelftoLeft();
		  info();
		  }
		  HAL_Delay(wait);
}

void Parking()
{
	Turning_180();
	Line_Forward();
}

void ChooseDirection()
{
	choose:
	data = HAL_ADC_GetValue(&hadc1);
	if (data < 1100)
	    {
		Target = 'A';
	    }
	else if (data > 1500 && data < 2200)
	    {
		Target = 'B';
	    }
	else if (data > 2800 && data < 3600)
		{
		Target = 'C';
		}
	else if (data > 3800)
		{
		Target = 'D';
		}
	else
	{
		for (int j=0; j<30; j++)
		{
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
			HAL_Delay(50);
		}
		HAL_Delay(1000);
		goto choose;
	}
}

void Master_Escape ()
{
	Turn_Left();
	Turn_Left();
	Line_Forward();
	Turning_180();
}

void M_Lamda ()
{
    check1:
    GetObjects();
    if (Distance < Short)
    {
    	//m lamda1
    	Stop();
    	HAL_Delay(duration);
    	goto check1;
    }
    else {
    	//m lamda0
    	Take_Forward();
    	Take_Off();
    	Turn_Right();
    }
}

void M_Angel ()
{
    check2:
    GetObjects();
    if (Distance < Long)
    {
    	//m Angel1
    	Stop();
    	HAL_Delay(duration);
    	goto check2;
    }
    else {
    	//m Angle0
    	Take_Forward();
    	Take_Off();
    	Turn_Right();
    }
}

void M_Linear ()
{
    check2:
    GetObjects();
    if (Distance < Long)
    {
    	//mL1
    	Stop();
    	HAL_Delay(duration);
    	goto check2;
    }
    else {
    	//m L0
    	Take_Forward();
    	Take_Off();
    }
}


void M_Finale ()
{
    GetObjects();
    if (Distance < Long)
    {
    	//m F1
    	Master_Escape();
    	HAL_Delay(duration);
    	check3:
    	GetObjects();
    	if (Distance < meter)
    	{
    		//m F2
    		goto check3;
    	}
    	else {
    		//m F3
    		Line_Forward();
    		Take_Forward();
    		Take_Forward();
    		Parking();
    	}
    }
    else {
    	//m F0
    	Line_Forward();
    	Take_Forward();
    	Parking();
    }
}


void Prime_Master ()
{
	M_Lamda();

    M_Angel();

    M_Finale();

}

void Binary_Master ()
{
    M_Lamda();

    M_Linear();

    M_Angel();

    M_Finale();
}

void Triple_Master()
{
    M_Lamda();

    M_Linear();

    M_Linear();

    M_Angel();

    M_Finale();
}

void Slave_Escape ()
{
   Turn_Right();
   Line_Forward();
   Turning_180();
}


void S_Lamda ()
{
	check1:
	GetObjects();
	if (Distance < Short)
	{
		//lamda1
		Stop();
		HAL_Delay(duration);
		goto check1;
	}
	else {
		//lamda0
		Take_Forward();
		Take_Off();
		Turn_Left();
	}
}

void S_Angle ()
{
	GetObjects();
	if (Distance < Long)
	{
		Stop();
		HAL_Delay(time);
		GetObjects();
		if (Distance < Long)
		{
			//Angel 1
			Slave_Escape();
			HAL_Delay(duration);
			//Angel 2
			GetObjects();
			while (Distance < Short)
			{

				Stop();
				GetObjects();
			}
			//Angel 3
			Line_Forward();
			Right_Forward();
            Take_Off();
            Turn_Left();
		}
		else {
			Line_Forward();
			Take_Off();
			Turn_Left();
		}
	}
	else
	{
		//Angel 0
		Line_Forward();
		Take_Off();
		Turn_Left();
	}
}

void Special_Angel()
{
	GetObjects();
	if (Distance < Short)
	{
		Stop();
		HAL_Delay(time);
		GetObjects();
		if (Distance < Short)
		{
			Slave_Escape();
			HAL_Delay(duration);
			GetObjects();
			while (Distance < Short)
			{
				Stop();
				GetObjects();
			}
			Line_Forward();
			Right_Forward();
            Take_Off();
            Turn_Left();
		}
		else {
			Line_Forward();
			Take_Off();
			Turn_Left();
		}
	}
	else
	{
		Line_Forward();
		Take_Off();
		Turn_Left();
	}
}


void S_Linear ()
{
	GetObjects();
	if (Distance < Long)
	{
		Stop();
		HAL_Delay(time);
		GetObjects();
		if (Distance < Long)
		{
			//L1
			Slave_Escape();
			HAL_Delay(duration);
			//L2
			GetObjects();
			while (Distance < Short)
			{
				Stop();
				GetObjects();
			}
			//L3
			Line_Forward();
			Right_Forward();
			Take_Off();
		}
		else {
			Line_Forward();
			Take_Off();
		}

	}
	else
	{
		//L0
		Line_Forward();
		Take_Off();
	}
}

void S_Finale ()
{
	GetObjects();
	if (Distance < Long)
	{
		//F1
		Turn_Right();
		Slave_Escape();
		//F2
		GetObjects();
		while (Distance < Long)
		{
			Stop();
			GetObjects();
		}
		//F3
		Line_Forward();
		Take_Forward();
		Take_Forward();
		Parking();
	}
	else
	{
		//F0
		Line_Forward();
		Take_Forward();
		Parking();
	}
}

void Prime_Slave ()
{
	S_Lamda();
	// At first point
    S_Angle();
	// At last point
    S_Finale();

}


void Binary_Slave ()
{
	S_Lamda();
	// At first point
	S_Linear();
	// At middle point
    S_Angle();
	// At last point
    S_Finale();
}

void Triple_Slave ()
{
	S_Lamda();
	// At first point
	S_Linear();
	// At second point
	S_Linear();
	// At third point
    S_Angle();
	// At last point
    S_Finale();
}

char Travel (current_state , next_state)
{
    switch (current_state) {
        case 'A':
            switch (next_state) {
            case 'A':
            	    Stop();
            	    return New_Loc = 'A';
            case 'B':
                    Prime_Master();
                    return New_Loc = 'B';
                    break;
            case 'C':
                    Binary_Master();
                	return New_Loc = 'C';
                    break;
            case 'D':
                    Triple_Master();
                	return New_Loc = 'D';
                    break;
            }
            break;
        case 'B':
            switch (next_state) {
            case 'A':
            	    S_Lamda();
            	    // At first point
                    Special_Angel();
            	    // At last point
                    S_Finale();
                    return New_Loc = 'A';
                    break;
            case 'B':
            	    Stop();
            	    return New_Loc = 'B';
            case 'C':
                    Prime_Master();
                    return New_Loc = 'C';
                    break;
            case 'D':
                    Binary_Master();
                	return New_Loc = 'D';
                    break;
            }
            break;
        case 'C':
            switch (next_state) {
            case 'A':
                    Binary_Slave();
                	return New_Loc = 'A';
                    break;
            case 'B':
                    Prime_Slave();
                	return New_Loc = 'B';
                    break;
            case 'C':
            	    Stop();
            	    return New_Loc = 'C';
            case 'D':
                    Prime_Master();
                	return New_Loc = 'D';
                    break;
            }
            break;
        case 'D':
            switch (next_state) {
            case 'A':
            	    S_Lamda();
            	    // At first point
            	    S_Linear();
            	    // At second point
            	    S_Linear();
            	    // At third point
                    Special_Angel();
            	    // At last point
                    S_Finale();
                	return New_Loc = 'A';
                    break;
            case 'B':
                    Binary_Slave();
                	return New_Loc = 'B';
                    break;
            case 'C':
                    Prime_Slave();
                	return New_Loc = 'C';
                    break;
            case 'D':
            	    Stop();
            	    return New_Loc = 'D';
            }
            break;
    }
}

  /* Infinite loop */

  while (1)
  {
	  button = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
	  pulse = button;
	  while (pulse == 1)
	  	  {
		    HAL_Delay(wait);
		    ChooseDirection();
		    Travel(Center, Target);
		    Center = New_Loc;

	  		pulse = 0;
	  	  }
  }

}
