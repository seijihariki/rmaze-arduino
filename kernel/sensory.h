#ifndef SENSORY_H_
#define SENSORY_H_

namespace sensory
{
    /* Returns raw distance for each useful sharp           */
    int distfr();
    int distfl();
    
    int distrf();
    int distrb();

    int distlf();
    int distlb();

    /* Returns raw average distance of sharps to that side  */ 
    float distf();
    float distr();
    float distl();

    /* Returns if has wall to that side.                    *
     *                                                      *
     * Params:                                              *
     * dst_sth: distance threshold for wall                 */
    bool wf(int dst_sth);
    bool wr(int dst_sth);
    bool wl(int dst_sth);

    /* Returns if tile is black                             *
     *                                                      *
     * Params:                                              *
     * col_sth: color threshold for detecting black         */
    bool isblack(int col_sth);

    /* Returns if should align                              *
     *                                                      *
     * Params:                                              *
     * lin_sth: error threshold for aligning linearly       *
     * rot_sth: error threshold for aligning rotationally   */
    bool shouldAlign(float lin_sth, float rot_sth);

    /* Returns if on ramp                                   *
     *                                                      *
     * Params:                                              *
     * max_angle: max angulation to consider not on ramp    */
    bool onRamp(float max_angle);
}

#endif
