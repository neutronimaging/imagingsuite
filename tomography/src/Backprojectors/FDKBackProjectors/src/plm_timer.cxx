/* -----------------------------------------------------------------------
   See COPYRIGHT.TXT and LICENSE.TXT for copyright and license information
   ----------------------------------------------------------------------- */
#include "plmsys_config.h"
#include <stdlib.h>
#include <stdio.h>

#include "plm_timer_p.h"
#include "plm_timer.h"

#include "compiler_warnings.h"

Plm_timer_private::Plm_timer_private ()
{
#if defined (_WIN32)
    QueryPerformanceFrequency (&this->clock_freq);
#endif
    acc_time = 0.;
    running = true;
}

double
Plm_timer_private::get_time ()
{
#if defined (_WIN32)
    LARGE_INTEGER clock_count;
    QueryPerformanceCounter (&clock_count);
    return ((double) (clock_count.QuadPart)) / ((double) this->clock_freq.QuadPart);
#else
    struct timeval tv;
    int rc;
    rc = gettimeofday (&tv, 0);
    UNUSED_VARIABLE (rc);
    return ((double) tv.tv_sec) + ((double) tv.tv_usec) / 1000000.;
#endif
}

double
Plm_timer_private::elapsed_time ()
{
    if (!this->running) {
        return 0.;
    }

    double current_time = this->get_time ();
    return current_time - this->start_time;
}

Plm_timer::Plm_timer ()
{
    this->d_ptr = new Plm_timer_private;
}

Plm_timer::~Plm_timer ()
{
    delete this->d_ptr;
}

void
Plm_timer::start ()
{
    d_ptr->acc_time = 0.;
    d_ptr->running = true;
    d_ptr->start_time = d_ptr->get_time ();
}

void
Plm_timer::stop ()
{
    if (d_ptr->running) {
        d_ptr->acc_time += d_ptr->elapsed_time ();
        d_ptr->running = false;
    }
}

void
Plm_timer::resume ()
{
    if (!d_ptr->running) {
        /* Don't reset acc_time */
        d_ptr->running = true;
        d_ptr->start_time = d_ptr->get_time ();
    }
}

double
Plm_timer::report ()
{
    return d_ptr->acc_time + d_ptr->elapsed_time();
}
