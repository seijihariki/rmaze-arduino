#ifndef MOVEMENT_H_
#define MOVEMENT_H_

namespace movement
{
	/* Aligns robot. Stops after tout (timeout) or if		*
	 * shouldn't align. (Using function from sensory)		*
	 *														*
	 * Params:												*
	 * tout: timeout (ms)									*
	 * dst_sth: distance threshold (raw number)				*
	 *														*/
	void align(int tout, int dst_sth);

	/* Stops all motors.									*/
	void stop();

	/* Applies walk front movement, returning sucessful.	*
	 * Must check MLX while walking.						*
	 * Params:												*
	 * mv: movement											*
	 * dst_sth: distance threshold (raw number)				*
	 *														*
	 * Ret:													*
	 * Sucessful moves.										*/
	move walkf(move mv, int dst_sth);

	/* Applies turn right movement, returning sucessful.	*
	 *														*
	 * Params:												*
	 * mv: movement											*
	 * dst_sth: distance threshold (raw number)				*
	 *														*
	 * Ret:													*
	 * Sucessful moves.										*/
	move turnl(move mv, int dst_sth);

	/* Applies turn left movement, returning sucessful.		*
	 *														*
	 * Params:												*
	 * mv: movement											*
	 * dst_sth: distance threshold (raw number)				*
	 *														*
	 * Ret:													*
	 * Sucessful moves.										*/
	move turnr(move mv, int dst_sth);

	/* Goes up ramp.										*/
	void upramp();

	/* Goes down ramp.										*/
	void downramp();

	/* Drop a kit.											 */
	void drop(int side);
}

#endif
