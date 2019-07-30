/*
 * Authors: Anthony Berbari, Malek Karray
*/

#include <stdbool.h>
#include <stdint.h>

/*
 * Initializes motors
 */
void initMotorControl(void);

/*
 *  Maintains a turn on a dime in the requested
 *  Args are 'L' or 'R' where L refers to CCW and R refers to CW
 */
void orientDirection(char dir);

/*
 * Maintains a forward trajectory
 *
 */
void driveForward(void);

/*
 * Stops motors, resets module
 */
void killAll(void);
