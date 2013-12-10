/* ************************************************************************** *
* Global variables: To stay organized as much as possible,
* 					 here are gathered the defines and global variables
* ************************************************************************** */


/*****************************************		Colors		****************************************/

/* Colors returned by SensorValue for a color sensor */
#define BLACK 1
#define BLUE 2
#define GREEN 3
#define RED 5
#define WHITE 6
#define NO_COLOR -1

/*****************************************		SPEED		****************************************/

/* Main speed of the motors */
#define SPEED 70
#define LOW_SPEED 36	// Ratio of the slowed motor during trackline

/* Speed to go to stock */
#define STOCK_SPEED 40
#define LOW_STOCK_SPEED 0
#define ADJUST_SPEED 5	// The speed to adjust the position on the red

/* Rotation speed */
#define ROT_SPEED 40
#define GO_BLACK 20		// the speed to adjust the position on the line after the main rotation

/*****************************************		Timers		***************************************************/

// Time of validation of a sensor value, to avoid artefacts (for main dots)
#define TIME_COL_DOT 50
// Refresh rate for dot sensor
#define REF_DOT 10
// Time of validation for stock dots
#define TIME_COL_ST 30

/*****************************************		Movement		*********************************************/

// Rotation
typedef enum {
	LEFT,
	RIGHT
} Direction;

/* Line following tasks */
typedef enum {
	GREEN_DOT_TASK,
	RED_DOT_TASK,
	GREEN_STOCK_TASK,
	RED_STOCK_TASK,
	START_TASK,
	CHRONO_TASK
} TaskType;


// encoding values for different rotation angles
#define ANGLE_90 200
#define ANGLE_180 450

/*****************************************		Environment		*********************************************/

/* Structure defining the robot's environment */
typedef struct {
	int blue, red, red_threshold, blue_threshold; /* Average value of the colors for the light sensor, to be calibrated FILE */
	int b_color; /* Current ball color NO_COLOR */
	int p; /* The p parameter for our own ball RED */

	bool line_stop; /* Sent by the task of the color sensor to stop the trackline */
	bool se; // = true; /* Wether we're working at SE or SPE */
	bool empty; // = false; /* Wether the stock is empty or not */
	int bse; // = 3; /* balls in SE */
	int left_col, right_col;
	int line_lost_time; /* Time after witch the robot lost the line */
} Environment;

Environment robot; /* Main environment */
