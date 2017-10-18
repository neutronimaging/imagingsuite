//*****************************************************************************
// Synchrotron SOLEIL
//
// 'Date' class
//
// Creating : 22/03/2005
// Author   : S. Poirier
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; version 2 of the License.
// 
// This program is distributed in the hope that it will be useful, but WITHOUT 
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//*****************************************************************************

#include "base.h"

#ifndef __DATE_H__
  #include "date.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#if defined(WIN32)
  #include <sys\timeb.h>
  #define _X86_
  #include <windef.h>
  #include <winbase.h>
  #include <winnls.h>
//  #include "syswin.cpp"
#elif defined(__LINUX__)
  #include <sys/time.h>
#endif

// standard library objets
#include <iostream>
#include <time.h>

/// Absolute reference : 0 january 4713 b JC at 12h ( -> -4712 for the calculations )
const int REF_YEAR = -4712;

/// Julian date for UNIX reference
const int JULIAN_REF_UNIX = 2440588;

// Error msgs
const char DATE_SET_ERR[]        = "Cannot Setting Date object";

// Reasons msgs
const char DATE_BAD_ARGS[]        = "Bad arguments";

using namespace soleil;

//============================================================================
// Internal functions
//============================================================================

// Number of days between 1th march 0 and 1th march iY
long DeltaDaysYear(int iY)
{
  return 365l * iY +
   ((long)iY/4l) -     // leap year
   ((long)iY/100l) +   // 100th are not leap
   ((long)iY/400l);    // 400th are leap years
}

// Number of days from 1th march to 1th <iM>
long DeltaDaysMonth(int iM)
{
  return (306l*iM + 4l)/10l;
}

//----------------------------------------------------------------------------
// Calculate julian date for a day at 12.
// piDeltaDays : pointer to delta with julian calendar without the reform (in 1582)
//----------------------------------------------------------------------------
long CalcJulianDate(int iY, int iM, int iD, int *piDeltaDays=NULL)
{
  int flGregLeap = 0;
  int flJulianLeap = 0;
  int iYMod = iY % 4;
  int iYFromRef = iY - REF_YEAR;
  int iDeltaDays = 0;
  // Number of days in Julian calendar
  long lNbDays = iYFromRef * 365L + ((iYFromRef + 3) / 4);

  // Leap year candidate
  if( iYMod == 0 )
  {
    flJulianLeap = 1;
    flGregLeap = 1;
  }

  // Year in Gregorian calendar
  // Applying reform : 
  if( iY > 1582 || (iY == 1582 && iM > 12 ) || (iY == 1582 && iM == 12 && iD > 19) )
    iDeltaDays = 10;

  // Calculating delta
  int iDM = 0;
  if( iY >= 1700 )
    iDM = -((iY - 1) / 100) + ((iY - 1) / 400) + 12;

  if( (iY % 400) && !(iY % 100) )
    flGregLeap = 0;

  iDeltaDays -= iDM;

  // Add number of days
  if( iM <= 8 )
    lNbDays += (iM-1) * 30 + iM/2;
  else
    lNbDays += (iM-1) * 30 + (iM-1) / 2 + 1;

  if( iM >= 3 && flGregLeap )
    lNbDays--;
  else if( iM >= 3 && !flGregLeap )
  {
    lNbDays += flJulianLeap - 2;
    iDeltaDays += flJulianLeap;
  }

  if( piDeltaDays )
    // Returns delta between Gregorian et Julian calandar
    *piDeltaDays = iDeltaDays;

  // Julian day in right calendar
  return lNbDays + iD - 1 - iDeltaDays;
}

//----------------------------------------------------------------------------
// Calculate Julian day date
//----------------------------------------------------------------------------
void JJToDate(long lJJ, int *piYear, uint *puiMonth, uint *puiDay)
{
  int iYear, iMonth=0, iDay;
  // Retreive year
  int iJulianYear = (int)(lJJ / 365.25);
  iYear =  iJulianYear + REF_YEAR;
  int iDeltaDays;
  // Retreive le julian day for first day of this year
  long lJJBegYear = CalcJulianDate(iYear, 1, 1, &iDeltaDays);

  // Position in respect to the reform
  int iDJ = int(lJJ - lJJBegYear);
  int flLeap = 0;

  if( lJJ < 2299226.5 )
  {
    iDJ -= iDeltaDays;

    if( (iYear % 4) == 0 )
      flLeap = 1;
  }
  else if( iYear == 1582 ) // && dJJ >= 2299226.5
    iDJ += 10;

  if( flLeap == 0 )
  {
    if( (iYear % 4) == 0 )
      flLeap = 1;
  }

  if( lJJ >= 2299226.5 && (iYear % 100) == 0 && (iYear % 400) != 0 )
    flLeap = 0;

  int iDM = 0;
  if( flLeap && iDJ == 59 )
  {
    // 29th februar
    iDM = 30;
    iMonth = 2;
  }

  if( flLeap && iDJ > 59 )
    // Substract leap day
    iDJ -= 1;

  // Retreive month
  iDJ -= 1;
  if( iDJ >= 333 )
  {
    iMonth = 12;
    iDM = 333;
  }
  else if( iDJ >= 303 )
  {
    iMonth = 11;
    iDM = 303;
  }
  else if( iDJ >= 272 )
  {
    iMonth = 10;
    iDM = 272;
  }
  else if( iDJ >= 242 )
  {
    iMonth = 9;
    iDM = 242;
  }
  else if( iDJ >= 211 )
  {
    iMonth = 8;
    iDM = 211;
  }
  else if( iDJ >= 180 )
  {
    iMonth = 7;
    iDM = 180;
  }
  else if( iDJ >= 150 )
  {
    iMonth = 6;
    iDM = 150;
  }
  else if( iDJ >= 119 )
  {
    iMonth = 5;
    iDM = 119;
  }
  else if( iDJ >= 89 )
  {
    iMonth = 4;
    iDM = 89;
  }
  else if( iDJ >= 58 && iDM == 0 )
  {
    iMonth = 3;
    iDM = 58;
  }
  else if( iDJ >= 30 && iDM == 0 )
  {
    iMonth = 2;
    iDM = 30;
  }
  else if( iDM == 0 )
  {
    iMonth = 1;
    iDM = -1;
  }

  // Retreive day
  iDay = iDJ - iDM + 1;

  if( iDJ > 333 && iDay > 31 )
  {
    iYear += 1;
    iDay -= 31;
    iMonth = 1;
  }

  *piYear = iYear;
  *puiMonth = iMonth;
  *puiDay = iDay;
}

//----------------------------------------------------------------------------
// TimeZoneBias
//----------------------------------------------------------------------------
double TimeZoneBias()
{
  #ifdef WIN32
    TIME_ZONE_INFORMATION TimeZoneInformation;
    GetTimeZoneInformation(&TimeZoneInformation);
    double dSec = - (double)TimeZoneInformation.Bias * SEC_PER_MIN;
    return dSec;
  #else
    struct timeval tv;
    #ifdef __SVR4__
      #ifdef _SVID_GETTOD
        gettimeofday(&tv);
      #else
        gettimeofday(&tv, NULL);
      #endif
    #else
      struct timezone tzp;
      gettimeofday(&tv, &tzp);
    #endif
 
    // Calculer l'heure universelle
    time_t tGM = tv.tv_sec;
    struct tm tmGM;    
    tGM = mktime(gmtime_r(&tGM, &tmGM));

    // Calculer l'heure locale
    time_t tLocal = tv.tv_sec;
    struct tm tmLocal;        
    tLocal = mktime(localtime_r(&tLocal, &tmLocal));

    double dDiff = difftime(tLocal, tGM);
    return dDiff;
  #endif
}

//=============================================================================
// SDateFields structure
//
//=============================================================================
//----------------------------------------------------------------------------
// SDateFields::Clear
//----------------------------------------------------------------------------
void SDateFields::Clear()
{
  memset(this, 0, sizeof(*this));
}

//----------------------------------------------------------------------------
// SDateFields::IsEmpty
//----------------------------------------------------------------------------
int SDateFields::IsEmpty() const
{
  return iYear == 0 && 
         uiMonth == 0 && 
         uiDay == 0 && 
         uiHour == 0 && 
         uiMin == 0 &&
         dSec == 0.;
}

//=============================================================================
// Date class
//
//=============================================================================

//----------------------------------------------------------------------------
// Date::NbDaysInMonth
//----------------------------------------------------------------------------
int Date::NbDaysInMonth(int iMonth, int iYear)
{
  switch( iMonth )
  {
    case 1: // january
    case 3: // march
    case 5: // may
    case 7: // july
    case 8: // august
    case 10:// october
    case 12:// december
      return 31;
   
    case 2: // februar : depend of year
      if( (iYear%4 == 0 && iYear < 1582) ||
           iYear%4 == 0 && ( (iYear%100) || !(iYear%400) ) )
        return 29; // leap years
      else 
        return 28; // ordinary years

    default : // other months
      return 30;
  }
}

//----------------------------------------------------------------------------
// Date::NbDaysInYear
//----------------------------------------------------------------------------
int Date::NbDaysInYear(int iYear)
{
  if( !(iYear%4) && ( (iYear%100) || !(iYear%400) ) )
    // Leap year
    return 366;

  return 365;
}

//----------------------------------------------------------------------------
// Date::MonthName
//----------------------------------------------------------------------------
pcsz Date::MonthName(int iMonth)
{
  if( iMonth > 1 && iMonth < 13 )
    return s_pszMonth[iMonth-1];

  return "";
}

//----------------------------------------------------------------------------
// Mise à l'heure courante
// - bUt : si vrai, on utilise l'heure universelle (UTC)
//----------------------------------------------------------------------------
void Date::SetCurrent(bool bUt)
{  
  #ifdef WIN32
    SYSTEMTIME sysTm;
    if( bUt )
      GetSystemTime(&sysTm);
    else
      GetLocalTime(&sysTm);
    Set(sysTm.wYear, sysTm.wMonth, sysTm.wDay, sysTm.wHour, sysTm.wMinute, 
        (double)sysTm.wSecond + ((double)sysTm.wMilliseconds)/1000.0);

  #else
    long lTm, lMs;
    struct timeval tv;
    struct timezone tzp;
    gettimeofday(&tv, &tzp);
    lTm = tv.tv_sec;
    lMs = tv.tv_usec/1000;

    // Convert from 'time_t' format to 'struct tm' format
    struct tm tmCurrent;
    if (bUt)
      gmtime_r(&lTm, &tmCurrent);
    else
      localtime_r(&lTm, &tmCurrent);

    Set(tmCurrent.tm_year+1900, tmCurrent.tm_mon+1, tmCurrent.tm_mday, tmCurrent.tm_hour, 
        tmCurrent.tm_min, (double)tmCurrent.tm_sec + ((double)lMs /1000.));
  #endif
}

//----------------------------------------------------------------------------
// Date::LocalToUT
//----------------------------------------------------------------------------
void Date::LocalToUT()
{
  *this -= TimeZoneBias();
}

//----------------------------------------------------------------------------
// Date::UTToLocal
//----------------------------------------------------------------------------
void Date::UTToLocal()
{
  *this += TimeZoneBias();
}

//----------------------------------------------------------------------------
// Date::Get
//----------------------------------------------------------------------------
void Date::Get(SDateFields *pDF) const
{              
  // Retreive Julian date
  long lJJ = JulianDate();

  // Day of week
  pDF->iDayOfWeek = (int)(lJJ % 7) + 2;
  if( pDF->iDayOfWeek > 7 )
    pDF->iDayOfWeek -= 7;

  // Set monday as first day of week
  pDF->iDayOfWeek -= 1;
  if( pDF->iDayOfWeek == 0 )
    pDF->iDayOfWeek = 7;

  // Retreive date
  JJToDate(lJJ, &pDF->iYear, &pDF->uiMonth, &pDF->uiDay);

  // Hour, minutes, seconds
  ulong ulSec = long((m_llDate / int64(MICROSEC_PER_SEC)) % int64(SEC_PER_DAY));
  pDF->uiHour = (uint)(ulSec / SEC_PER_HOUR);
  ulSec -= pDF->uiHour * SEC_PER_HOUR;
  pDF->uiMin = (uint)(ulSec / SEC_PER_MIN);
  int iSec = (uint)(ulSec - pDF->uiMin * SEC_PER_MIN);
  // Retreive micro-seconds
  double dMicro = double(m_llDate % MICROSEC_PER_SEC);
  pDF->dSec = iSec + dMicro / (double)MICROSEC_PER_SEC;

  // Calculate day of year
  pDF->iDayOfYear = (int)(JulianDate() - CalcJulianDate(pDF->iYear, 1, 1)) + 1;

  // Week of year
  int iDay = pDF->iDayOfYear - pDF->iDayOfWeek;
  pDF->iWeekOfYear = iDay / 7; 
  if( iDay >= 0 )
    pDF->iWeekOfYear++;
}

//----------------------------------------------------------------------------
// Date::Set
//----------------------------------------------------------------------------
void Date::Set(const SDateFields &df)
{
  if( df.IsEmpty() )
    // Empty date
    m_llDate = 0;
  else
  {
    uint uiMonth = df.uiMonth;
    int iYear  = df.iYear;

    // Check fields
    if( uiMonth > 12 ||  
        df.uiDay < 1 || df.uiDay > (uint)NbDaysInMonth(uiMonth, iYear) || 
        df.uiHour > 24 ||
        df.uiMin > 59 || 
        df.dSec < 0 || df.dSec >= 60. )
    {
      throw DateException(DATE_SET_ERR, DATE_BAD_ARGS, "Date::Set");
    }

    // Julian day at 12.
    double dJJ = CalcJulianDate(df.iYear, df.uiMonth , df.uiDay);
    m_llDate = int64(dJJ * SEC_PER_DAY) * int64(MICROSEC_PER_SEC) + 
               int64(df.uiHour * SEC_PER_HOUR) * int64(MICROSEC_PER_SEC) +
               int64(df.uiMin * SEC_PER_MIN) * int64(MICROSEC_PER_SEC) +
               int64(df.dSec * MICROSEC_PER_SEC);
  }
}
 
//----------------------------------------------------------------------------
// Date::Set
//----------------------------------------------------------------------------
void Date::Set(int iYear, uint uiMonth, uint uiDay, 
                uint uiHour, uint uiMin, double dSec)
{
  SDateFields df;
  df.iYear  = iYear;
  df.uiMonth = uiMonth;
  df.uiDay   = uiDay;
  df.uiHour  = uiHour;
  df.uiMin   = uiMin;
  df.dSec   = dSec;
  Set(df);
}

//----------------------------------------------------------------------------
// Date::SetDayOfYear
//----------------------------------------------------------------------------
void Date::SetDayOfYear(int iDayOfYear, int iYear)
{
  SDateFields df;
  Get(&df);
  Set(iYear, 1, 1, df.uiHour, df.uiMin, df.dSec);
  AddSec((iDayOfYear - 1) * SEC_PER_DAY);
}

// Macro for implementing access to date fields
#define IMPLEMENT_DATEFIELD(Name)     \
void Date::Set##Name(uint ui)          \
{                                     \
  SDateFields df;                     \
  Get(&df);                           \
  df.ui##Name = ui;                     \
  Set(df);                            \
}                                     \
                                      \
uint Date::Name() const               \
{                                     \
  SDateFields df;                     \
  Get(&df);                           \
  return df.ui##Name;                  \
}                                     

IMPLEMENT_DATEFIELD(Min)
IMPLEMENT_DATEFIELD(Hour)
IMPLEMENT_DATEFIELD(Day)
IMPLEMENT_DATEFIELD(Month)

//----------------------------------------------------------------------------
// Date::SetYear
//----------------------------------------------------------------------------
void Date::SetYear(int iYear)
{
  SDateFields df;
  Get(&df);
  df.iYear = iYear;
  Set(df);
}

//----------------------------------------------------------------------------
// Date::Year
//----------------------------------------------------------------------------
int Date::Year() const
{
  SDateFields df;
  Get(&df);
  return df.iYear;
}
 
//----------------------------------------------------------------------------
// Date::SetSec
//----------------------------------------------------------------------------
void Date::SetSec(double dSec)
{
  SDateFields df;
  Get(&df);
  df.dSec = dSec;
  Set(df);
}

//----------------------------------------------------------------------------
// Date::Sec 
//----------------------------------------------------------------------------
double Date::Sec() const
{
  SDateFields df;
  Get(&df);
  return df.dSec;
}

//----------------------------------------------------------------------------
// Date::Micro
//----------------------------------------------------------------------------
long Date::Micro() const
{
  return long(m_llDate % MICROSEC_PER_SEC);
}

//----------------------------------------------------------------------------
// Date::Ms
//----------------------------------------------------------------------------
long Date::Ms() const
{
  return long(m_llDate % MICROSEC_PER_SEC) / 1000;
}

//----------------------------------------------------------------------------
// Date::JulianDate
//----------------------------------------------------------------------------
long Date::JulianDate() const
{
  return long(m_llDate / MICROSEC_PER_DAY);
}

//----------------------------------------------------------------------------
// Date::SetJulianDate
//----------------------------------------------------------------------------
void Date::SetJulianDate(long lJulianDate)
{
  m_llDate = lJulianDate * MICROSEC_PER_DAY;
}

//----------------------------------------------------------------------------
// Date::DayOfWeek 
//----------------------------------------------------------------------------
int Date::DayOfWeek() const
{
  SDateFields df;
  Get(&df);
  return df.iDayOfWeek;
}

//----------------------------------------------------------------------------
// Date::DayOfYear
//----------------------------------------------------------------------------
int Date::DayOfYear() const
{
  long lJStartDate = CalcJulianDate(Year(), 1, 1);
  return (int)(JulianDate() - lJStartDate) + 1;
}

//----------------------------------------------------------------------------
// Date::WeekOfYear
//----------------------------------------------------------------------------
int Date::WeekOfYear() const
{
  SDateFields df;
  Get(&df);
  return df.iWeekOfYear;
}

//----------------------------------------------------------------------------
// Date::LongUnix()
//----------------------------------------------------------------------------
long Date::LongUnix() const
{
  return (long)(((m_llDate / MICROSEC_PER_SEC) - int64(JULIAN_REF_UNIX) * int64(SEC_PER_DAY)));
}

//----------------------------------------------------------------------------
// Date::SetLongUnix
//----------------------------------------------------------------------------
void Date::SetLongUnix(long lRefSec)
{
  m_llDate = (int64(JULIAN_REF_UNIX) * int64(SEC_PER_DAY) + int64(lRefSec)) * int64(MICROSEC_PER_SEC);
}

//----------------------------------------------------------------------------
// Date::DoubleUnix()
//----------------------------------------------------------------------------
double Date::DoubleUnix() const
{
  return double(((m_llDate / MICROSEC_PER_SEC) - int64(JULIAN_REF_UNIX) * int64(SEC_PER_DAY)))
         + Ms() / 1000.0;
}

//----------------------------------------------------------------------------
// Date::SetDoubleUnix
//----------------------------------------------------------------------------
void Date::SetDoubleUnix(double dRefSec)
{
  m_llDate = (int64(JULIAN_REF_UNIX) * int64(SEC_PER_DAY) * int64(1000) + 
             int64(dRefSec * 1000)) * int64(1000);
}

//----------------------------------------------------------------------------
// Date::ISO8601
//----------------------------------------------------------------------------
String Date::ToLocalISO8601() const
{
  String strDate;
  String strFmt;

  double dTZ = TimeZoneBias();
  if( dTZ < 0 )
    strFmt = "%04d-%02d-%02dT%02d:%02d:%02d-%02d%02d";
  else
    strFmt = "%04d-%02d-%02dT%02d:%02d:%02d+%02d%02d";

  // Prevent any rounding error
  if( dTZ < 0 )
    dTZ = -dTZ;

  int iSec = int(dTZ + 0.5);
   
  int iHourBias = iSec / 3600;
  int iMinBias = (iSec - (3600 * iHourBias)) / 60;

  // Split date into fields
  SDateFields df;
  Get(&df);
  
  strDate = StrFormat(PSZ(strFmt), df.iYear, df.uiMonth, df.uiDay,
                              df.uiHour, df.uiMin, long(df.dSec + 0.5), iHourBias, iMinBias);
  return strDate;
}

//----------------------------------------------------------------------------
// Date::ToISO8601
//----------------------------------------------------------------------------
String Date::ToISO8601() const
{
  String strDate;
  String strFmt = "%04d-%02d-%02dT%02d:%02d:%02dZ";

  // Split date into fields
  SDateFields df;
  Get(&df);
  
  strDate = StrFormat(PSZ(strFmt), df.iYear, df.uiMonth, df.uiDay,
                              df.uiHour, df.uiMin, long(df.dSec + 0.5));
  return strDate;
}

//----------------------------------------------------------------------------
// Date::ToISO8601ms
//----------------------------------------------------------------------------
String Date::ToISO8601msTU() const
{
  String strDate;
  String strFmt = "%04d-%02d-%02dT%02d:%02d:%06.3lfZ";

  // Split date into fields
  SDateFields df;
  Get(&df);
  
  strDate = StrFormat(PSZ(strFmt), df.iYear, df.uiMonth, df.uiDay,
                              df.uiHour, df.uiMin, df.dSec);
  return strDate;
}

//----------------------------------------------------------------------------
// Date::ToISO8601ms
//----------------------------------------------------------------------------
String Date::ToISO8601ms() const
{
  String strDate;
  String strFmt;

  strFmt = "%04d-%02d-%02dT%02d:%02d:%06.3lf";

  // Split date into fields
  SDateFields df;
  Get(&df);
  
  strDate = StrFormat(PSZ(strFmt), df.iYear, df.uiMonth, df.uiDay,
                              df.uiHour, df.uiMin, df.dSec);
  return strDate;
}

//----------------------------------------------------------------------------
// Date::ToLocalISO8601ms
//----------------------------------------------------------------------------
String Date::ToLocalISO8601ms() const
{
  String strDate;
  String strFmt;

  double dTZ = TimeZoneBias();
  if( dTZ < 0 )
    strFmt = "%04d-%02d-%02dT%02d:%02d:%06.3lf-%02d%02d";
  else
    strFmt = "%04d-%02d-%02dT%02d:%02d:%06.3lf+%02d%02d";

  // Prevent any rounding error
  if( dTZ < 0 )
    dTZ = -dTZ;

  int iSec = int(dTZ + 0.5);
   
  int iHourBias = iSec / 3600;
  int iMinBias = (iSec - (3600 * iHourBias)) / 60;

  // Split date into fields
  SDateFields df;
  Get(&df);
  
  strDate = StrFormat(PSZ(strFmt), df.iYear, df.uiMonth, df.uiDay,
                              df.uiHour, df.uiMin, df.dSec, iHourBias, iMinBias);
  return strDate;
}

//----------------------------------------------------------------------------
// Date::ToInter
//----------------------------------------------------------------------------
String Date::ToInter(bool bMillis) const
{
  String strDate;
  String strFmt;

  if( bMillis )
    strFmt = "%04d-%02d-%02d %02d:%02d:%06.3lf";
  else
    strFmt = "%04d-%02d-%02d %02d:%02d:%02d";

  // Split date into fields
  SDateFields df;
  Get(&df);
  
  if( bMillis )
    strDate = StrFormat(PSZ(strFmt), df.iYear, df.uiMonth, df.uiDay,
                              df.uiHour, df.uiMin, df.dSec);
  else
    strDate = StrFormat(PSZ(strFmt), df.iYear, df.uiMonth, df.uiDay,
                              df.uiHour, df.uiMin, int(df.dSec+0.5));

  return strDate;
}

//----------------------------------------------------------------------------
// Date::UnixTime
//----------------------------------------------------------------------------
ulong Date::UnixTime()
{
  return CurrentDate().LongUnix();
}

//===========================================================================
//
// CurrentDate
//
//===========================================================================
//----------------------------------------------------------------------------
// CurrentDate::CurrentDate
//----------------------------------------------------------------------------
CurrentDate::CurrentDate(bool bUT)
{
  SetCurrent(bUT);
}

//===========================================================================
//
// CurrentDate
//
//===========================================================================
//----------------------------------------------------------------------------
// DateVariableEvaluator::DateVariableEvaluator
//----------------------------------------------------------------------------
DateVariableEvaluator::DateVariableEvaluator(const Date &aDate) :
m_tDate(aDate), m_bFixedDate(true), m_bUT(false)
{

}
DateVariableEvaluator::DateVariableEvaluator(bool bFixedDate, bool bUT) :
m_bFixedDate(bFixedDate), m_bUT(bUT)
{
  if( bFixedDate )
    m_tDate.SetCurrent();
}

//----------------------------------------------------------------------------
// DateVariableEvaluator::DateVariableEvaluator
//----------------------------------------------------------------------------
bool DateVariableEvaluator::Evaluate(String *pstrVar)
{
  if( !m_bFixedDate )
    m_tDate.SetCurrent(m_bUT);

  if( IsEqualsNoCase(*pstrVar, "ISO8601" ) )
  {
    *pstrVar = m_tDate.ToLocalISO8601();
    return true;
  }

  else if( IsEqualsNoCase(*pstrVar, "UNIXDATE" ) )
  {
    *pstrVar = StrFormat("%ld", m_tDate.LongUnix());
    return true;
  }

  SDateFields df;
  m_tDate.Get(&df);

  if( IsEqualsNoCase(*pstrVar, "YEAR" ) )
  {
    *pstrVar = StrFormat("%d", df.iYear);
    return true;
  }

  else if( IsEqualsNoCase(*pstrVar, "YEAR100" ) )
  {
    *pstrVar = StrFormat("%02d", df.iYear % 100);
    return true;
  }

  else if( IsEqualsNoCase(*pstrVar, "MONTH" ) )
  {
    *pstrVar = StrFormat("%02d", df.uiMonth);
    return true;
  }

  else if( IsEqualsNoCase(*pstrVar, "DAY" ) )
  {
    *pstrVar = StrFormat("%02d", df.uiDay);
    return true;
  }

  else if( IsEqualsNoCase(*pstrVar, "DAY-OF-YEAR" ) )
  {
    *pstrVar = StrFormat("%03d", df.iDayOfYear);
    return true;
  }

  else if( IsEqualsNoCase(*pstrVar, "HOUR" ) )
  {
    *pstrVar = StrFormat("%02d", df.uiHour);
    return true;
  }

  else if( IsEqualsNoCase(*pstrVar, "MIN" ) )
  {
    *pstrVar = StrFormat("%02d", df.uiMin);
    return true;
  }

  else if( IsEqualsNoCase(*pstrVar, "SEC" ) )
  {
    *pstrVar = StrFormat("%02d", int(df.dSec + 0.5));
    return true;
  }

  return false;
}
