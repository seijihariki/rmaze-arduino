#ifndef SENSORY_H_
#define SENSORY_H_

/* Returns raw distance for each useful sharp           */
int sn_distfr();
int sn_distfl();
  
int sn_distrf();
int sn_distrb();

int sn_distlf();
int sn_distlb();

/* Returns raw average distance of sharps to that side  */ 
float sn_distf();
float sn_distr();
float sn_distl();

/* Returns if has wall to that side.                    *
 *                                                      *
 * Params:                                              *
 * dst_sth: distance threshold for wall                 */
bool sn_wf(int dst_sth);
bool sn_wr(int dst_sth);
bool sn_wl(int dst_sth);

/* Returns if tile is black                             *
 *                                                      *
 * Params:                                              *
 * col_sth: color threshold for detecting black         */
bool sn_isblack(int col_sth);

/* Returns if should align                              *
 *                                                      *
 * Params:                                              *
 * lin_sth: error threshold for aligning linearly       *
 * rot_sth: error threshold for aligning rotationally   */
bool sn_shouldAlign(float lin_sth, float rot_sth);

/* Returns if on ramp                                   *
 *                                                      *
 * Params:                                              *
 * max_angle: max angulation to consider not on ramp    */
bool sn_onRamp(float max_angle);



#endif
