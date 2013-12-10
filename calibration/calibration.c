#pragma config(Sensor, S1,     left_sensor,    sensorCOLORFULL)
#pragma config(Sensor, S2,     right_sensor,   sensorCOLORFULL)
#pragma config(Sensor, S3,     light_sensor,   sensorLightActive)
#pragma config(Sensor, S4,     sonarSensor,    sensorSONAR)
/*********************************************************************************************/

#include "../AI/global_variables.h"
#include "../AI/primary_tasks.c"
#include "../AI/composed_tasks.c"

// To calibrate the light values of red and blue balls
task main()
{
	short red, blue, no_color;
	short blue_threshold, red_threshold;
	short time;
	int i;
	TFileHandle file;
	TFileIOResult result;
	// We'll store the result in this file so the IA can use it
	string fname = "calibration.txt";
	int fsize = 100;

	// Open the arm and wait for a red ball
	ballDeprehension();
	nxtDisplayTextLine(1, "Balle ROUGE");
	nxtDisplayTextLine(2, "Appuyer bouton");
	nxtDisplayTextLine(3, "Orange.");

	while (nNxtButtonPressed != kEnterButton);

	ballPrehension();
	wait1Msec(250);
	// for a better result we use an average of 20 values
	red = SensorValue[light_sensor];
	for(i = 0; i < 19; i++)
		red += SensorValue[light_sensor];
	red /= 20;

	//display the value so we can check its correctness
	nxtDisplayTextLine(5, "ROUGE %i", red);
	wait1Msec(1500);

	// same for blue
	ballDeprehension();
	nxtDisplayTextLine(1, "Balle BLEUE");
	while (nNxtButtonPressed != kEnterButton);
	ballPrehension();
	wait1Msec(250);
	blue = SensorValue[light_sensor];
	for(i = 0; i < 19; i++)
		blue += SensorValue[light_sensor];
	blue /= 20;

	nxtDisplayTextLine(6, "BLEU %i", blue);
	wait1Msec(1500);

	// Now we check the value when their is no ball
	ballDeprehension();
	nxtDisplayTextLine(1, "Aucune balle");
	while (nNxtButtonPressed != kEnterButton);
	wait1Msec(1500);
	no_color = SensorValue[light_sensor];
	for(i = 0; i < 19; i++)
		no_color += SensorValue[light_sensor];
	no_color /= 20;

	// Go back to the rest state
	ballPrehension();

	/* 
	 * To determine wether a ball is blue or red or if their is no ball
	 * we'll use a threshold
	 */
	blue_threshold = abs((blue - no_color) / 2);
	red_threshold = abs((red - no_color) / 2);

	/* 
	 * We display all values so we can determine if their is a problem
	 * for example if a color value is to close of the empty value
	 */
	nxtDisplayTextLine(1, "ROUGE %i", red);
	nxtDisplayTextLine(2, "BLEU %i", blue);
	nxtDisplayTextLine(3, "RIEN %i", no_color);
	nxtDisplayTextLine(4, "SEUIL R %i", red_threshold);
	nxtDisplayTextLine(5, "SEUIL B %i", blue_threshold);

	nxtDisplayTextLine(6, "Appuyer bouton");
	nxtDisplayTextLine(7, "Orange.");
	wait1Msec(450);

	time = 0;

	// save everything in the file
	Delete(fname, result);
	OpenWrite(file, result, fname, fsize);
	WriteShort(file, result, red);
	WriteByte(file, result, ' ');
	WriteShort(file, result, blue);
	WriteByte(file, result, ' ');
	WriteShort(file, result, red_threshold);
	WriteByte(file, result, ' ');
	WriteShort(file, result, blue_threshold);
	WriteByte(file, result, ' ');
	WriteShort(file, result, 3 * time / 4);
	Close(file, result);

	nxtDisplayTextLine(6, "Appuyer bouton");
	nxtDisplayTextLine(7, "Orange.");
	while (nNxtButtonPressed != kEnterButton);
}
