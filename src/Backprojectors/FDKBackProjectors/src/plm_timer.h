/* -----------------------------------------------------------------------
   See COPYRIGHT.TXT and LICENSE.TXT for copyright and license information
   ----------------------------------------------------------------------- */
#ifndef _plm_timer_h_
#define _plm_timer_h_

//#include "plmsys_config.h"

class Plm_timer_private;

class PLMSYS_API Plm_timer {
public:
    Plm_timer ();
    ~Plm_timer ();

    void start ();
    void stop ();
    void resume ();
    double report ();
private:
    Plm_timer_private *d_ptr;
};

#endif
