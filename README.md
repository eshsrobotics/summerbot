# summerbot
Repository for Operation: S.U.M.M.E.R.B.O.T., a hobby robot built by the ESHS P.O.T.A.T.O.E.S. robotics team.


Wiring for summerbot
====================

1. B/VCC for wiring
	- Bottom pin for ground (black wire) goes to an Arduino ground pin
	- Top pin for signal
		1. Channel 1 (blue wire) is signal for right stick horizontal
		2. Channel 2 (white wire) is signal for right stick vertical
	- Middle pin for VCC (red wire) to 5V pin on Arduino

![FS-iA6B pinout](./500px-FlySky_IA6B_pinout.jpg)

2. PWM INPUT PINS
	- PWM input pin horizontal (blue wire) goes to arduino plugin 8
	- PWM input pin vertical (white wire) goes to arduino plugin 11


Arcade drive formulas
=====================


2. [Arcade Drive Pseudocode](https://www.chiefdelphi.com/uploads/default/original/3X/d/5/d57c217c08415b540ff1fdf6351a98e6c4ee5cd3.pdf), from [Ether](https://www.chiefdelphi.com/t/paper-arcade-drive/168720)

    ```c++
        /* FWD is the Forward commandRCW is the Rotate Clockwise command*/
        max = fabs(FWD);
    	if (fabs(RCW)>max) max = fabs(RCW);
    	sum = FWD+RCW;
    	dif = FWD-RCW;
    	if(FWD>=0){
    	  if(RCW>=0) { L = max; R = dif; } else { L = sum; R = max; }
    	}else{
    	  if(RCW>=0) { L = sum; R = -max; }else { L = -max; R = dif; }
    	}
    ```

    Translated into more readable pseudocode, we have:

	```
	    # Turn and Drive are numbers between -100% and 100%.
		# A drive of +100% drives forward at full speed.
		# A turn of +100% rotates clockwise at full speed.
	    FUNCTION Arcade(drive, turn) RETURNS (left, right)
		    maximum = MAX(ABS(drive), ABS(turn))
			sum = drive + turn
			difference = drive - turn
			IF drive >= 0 THEN
			    IF turn >= 0 THEN				    
					return (maximum, difference)  # First quadrant.
				ELSE
					return (sum, maximum)         # Second quadrant.
				END IF
			ELSE
				IF turn >= 0 THEN
				    return (sum, -maximum)        # Fourth quadrant.
				ELSE
				    return (-maximum, difference) # Third quadrant.
				END IF
			END IF					
		END FUNCTION
	```
	Some numbers:
	|| Drive || Turn || Left || Right ||
	| 0 | 0 | 0 | 0 |
	| 25% | 0% | 25% | 25% |
	| 0% | -25% | -25% | 25% |
	| +50% | +50% | +50% | 0% |
	| +50% | +100% | +100% | -50% |
	| -100% | 50% | -50% | -100% |
