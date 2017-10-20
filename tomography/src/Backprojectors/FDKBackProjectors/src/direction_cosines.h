/* -----------------------------------------------------------------------
   See COPYRIGHT.TXT and LICENSE.TXT for copyright and license information
   ----------------------------------------------------------------------- */
#ifndef _direction_cosines_h_
#define _direction_cosines_h_

//#include "plmbase_config.h"
#include <string>
#include <stdio.h>
//#include "itk_image.h"
#include "plm_math.h"

#define DIRECTION_COSINES_IDENTITY_THRESH 1e-9

namespace itk { template<class T, unsigned int NRows, unsigned int NColumns> class Matrix; }
typedef itk::Matrix < double, 3, 3 > DirectionType;

class Direction_cosines_private;

class  Direction_cosines {
  public:
    Direction_cosines_private *d_ptr;

  public:
    Direction_cosines ();
    Direction_cosines (const DirectionType& itk_dc);
    ~Direction_cosines ();

  public:
    operator const float* () const;
    operator float* ();
  public:
    void set_identity ();

    /* Presets */
    void set_rotated_1 ();
    void set_rotated_2 ();
    void set_rotated_3 ();
    void set_skewed ();

    const float* get () const;
    float* get ();
    const float* get_inverse () const;
    void set (const float dc[]);
    void set (const DirectionType& itk_dc);
    bool set_from_string (std::string& str);
    bool is_identity ();
protected:
    void solve_inverse ();
};

#endif
