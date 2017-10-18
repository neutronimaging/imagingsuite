//*****************************************************************************
// Synchrotron SOLEIL
//
// 'Date' class
//
// Creating : 22/03/2005
// Author   : Stephane Poirier
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

#ifndef __DATE_H__
#define __DATE_H__

#ifndef __BASE_H__
  #include "base.h"
#endif

namespace gdshare
{

//============================================================================
// Constantes
//============================================================================

// Usual durations
#define SEC_PER_MIN         60L
#define SEC_PER_HOUR        3600L
#define SEC_PER_DAY         86400L
#define SEC_PER_MONTH       (SEC_PER_DAY*30)     // logical month (30 days)
#define SEC_PER_YEAR        (SEC_PER_DAY*30*12)  // logical year (12 logicals months)

#ifndef MS_SEC
  #define MS_SEC              1000L              // milliseconds per second
#endif
#define MICROSEC_PER_SEC      1000000L           // microseconds per second

#define MICROSEC_PER_DAY_H    20L
#define MICROSEC_PER_DAY_L    500654080UL
#define MICROSEC_PER_DAY      int64FromHLPair(MICROSEC_PER_DAY_H, MICROSEC_PER_DAY_L) // microseconds per day 

#ifndef MS_OVERFLOW
  // Nombre de ms indiquant un depassement de capacite d'un int64 lors du calcul
  // de difference entre deux dates
  #define MS_OVERFLOW	(int64FromHLPair(0x80000000, 0x0) - int64(1))
#endif

// 1970/01/01 at 0h
#define REF_UNIX        int64FromHLPair(0x2ed263d, 0x83a88000)

// Flags pour les noms de jours, mois et unités
// - on a 4 combinaisons pour les jours/mois et 6 pour les unités avec le pluriel
// - ABBR, LONG, LONGPL, OTHERS sont exclusifs ; on peut ajouter inter
#define TM_ABBR     0 // pas vraiment utile
#define TM_INTER    1 // international
#define TM_LONG     2 // long
#define TM_LONGPL   4 // long pluriel (pour les durées)
#define TM_OTHERS   6 // autres abbréviations autorisées en parsing (séparées par |)
#define TM_DEFAULT 16 // unité par défaut (pour les durées)

// identifiant des unités dans le tableau
#define TS_UNIT_SEC   0
#define TS_UNIT_MIN   1
#define TS_UNIT_HOUR  2
#define TS_UNIT_DAY   3
#define TS_UNIT_MONTH 4  // mois logique (=30 jours)
#define TS_UNIT_YEAR  5  // année logique (=12 mois logiques)

// Month names
static const pcsz s_pszMonth[] = 
{
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

//=============================================================================
/// Date exceptions
///
/// This class is designed to hold date exceptions
//=============================================================================
class DateException : public ExceptionBase
{
  const char *ErrorTitle()
  {
    return "Date class Error: ";
  }
public:
  DateException(const char *pcszError, const char *pcszReason, const char *pcszMethod):
  ExceptionBase(pcszError, pcszReason, pcszMethod)
  { }
};

//============================================================================
/// SDateFields
/// Date splitted in fields
//============================================================================
struct SDateFields
{
  int iYear;
  uint uiMonth;
  uint uiDay;

  uint uiHour;
  uint uiMin;
  double dSec;      // Seconds with microsec precision

  int iDayOfYear;   // Day of year (1-366)
  int iDayOfWeek;   // Day of week (1=lundi, 7=dimanche)
  int iWeekOfYear;  // Week of year (1-53)

  /// Clears all field
  void Clear();

  /// Accessor
  int IsEmpty() const;
};

//===========================================================================
/// The class CDate represents a specific instant in time, with 
/// microsecond precision
//===========================================================================
class Date
{
private:
  int64 m_llDate;   // Complete date with microsec precision

  // For debugging purpose only
  // NEVER USE THIS METHOD IN REAL CODE !!
  void txt() const;

public:
  /// Constructors
  Date() { m_llDate = 0; }

  /// Constructor from fields
  ///
  /// @param iYear Year from -4712 to ?
  /// @param uiMonth Month in range [1, 12]
  /// @param uiDay Day in range [1, 31]
  /// @param uiHour Hour in range [0, 23]
  /// @param uiMin Minute in range [0, 59]
  /// @param dSec Seconds in range [0, 59] with microsec precision
  /// 
  Date(int iYear, uint uiMonth, uint uiDay, uint uiHour=0, uint uiMin=0, double dSec=0)
  { Set(iYear, uiMonth, uiDay, uiHour, uiMin, dSec); }

  //-----------------------------------------------------------------
  /// @name Accessors
  //@{

  /// Returns data internal value
  int64 RawValue() const { return m_llDate; }
  void SetRawValue(int64 i) { m_llDate = i; }

  /// return 'true' if date is empty
  bool IsEmpty() const { return m_llDate == 0; }

  /// return 'true' if date is 1970/01/01 0h
  int IsEmptyUnix() const { return REF_UNIX == m_llDate; }

  /// Splittes date in fields
  ///
  /// @param pTm structure to fill in
  ///
  void Get(SDateFields *pTm) const;

  /// Returns the microseconds part
  long Micro() const;

  /// Returns the milliseconds part
  long Ms() const;

  /// Returns the seconds part with microsecond precision
  double Sec() const;

  /// Returns the minutes part
  uint Min() const;

  /// Return the hour part
  uint Hour() const;

  /// Returns the Day part
  uint Day() const;

  /// Returns the hour part
  uint Month() const;

  /// Return the year part
  int Year() const;

  /// Returns the day number of the week, starting from monday (1) to sunday (7)
  int DayOfWeek() const;

  /// Returns the day number in the range [1, 366]
  int DayOfYear() const;

  /// Returns the week number in the range [1, 53]
  int WeekOfYear() const;

  /// Gets Julian day at 12h
  long JulianDate() const;  // Jour julien de la date a 12h

  /// Gets real Julian day with respect with time
  double JulianDay() const;

  //@} Accessors

  //-----------------------------------------------------------------
  /// @name Setting date
  //@{

  /// Empties Date
  void SetEmpty() { m_llDate = 0; }

  /// Empties Date in unix sense
  void SetEmptyUnix() { m_llDate = REF_UNIX; }

  /// Initialize date
  ///
  /// @param sTm structure containing splitted date
  ///
  void Set(const SDateFields& sTm);

  /// Initializes date from explicit values
  ///
  /// @param iYear Year from -4712 to ?
  /// @param uiMonth Month in range [1, 12]
  /// @param uiDay Day in range [1, 31]
  /// @param uiHour Hour in range [0, 23]
  /// @param uiMin Minute in range [0, 59]
  /// @param dSec Seconds in range [0, 59] with microsec precision
  ///
  void Set(int iYear, uint uiMonth, uint uiDay, 
           uint uiHour=0, uint uiMin=0, double dSec=0);

  // Fixe la partie date depuis le numéro de jour dans l'année / clear le time
  void SetDayOfYear(int iDayOfYear, int iYear);

  /// Sets the seconds part with microsecond precision
  void SetSec(double dSec);

  /// Sets the minute part
  void SetMin(uint uiMin);

  /// Sets the hour part
  void SetHour(uint uiHour);

  /// Sets the day part
  void SetDay(uint uiDay);

  /// Sets the month part
  void SetMonth(uint uiMonth);

  /// Sets the year part
  void SetYear(int iYear);

  /// Sets the Julian date
  void SetJulianDate(long lJulianDate);

  /// Sets the internal value
  void Set(int64 ui64) { m_llDate = ui64; }

  /// Clears the time part (hour, min, sec)
  void ClearTime();

  /// Clears the date part (year, month, day)
  void ClearDate();
  
  /// Sets to current time
  /// 
  /// @param bUT if true sets to the universal time (UTC)
  ///        otherwise sets to local time
  void SetCurrent(bool bUT=false);

  /// Convert from local time to universal time (UTC)
  void LocalToUT();

  /// Convert from universal time (UTC) to local time
  void UTToLocal();

  /// Adds seconds
  void AddSec(double dSec) { m_llDate += int64(dSec * 1e6); }
  void operator +=(double dSec) { AddSec(dSec); }
  void operator -=(double dSec) { AddSec(-dSec); }
  
  //@}

  //-----------------------------------------------------------------
  /// @name Text methods
  //@{

  /// Gets date from a ISO8601 string
  void FromISO8601(const char* pszISO8601);

  /// Gets local time in ISO8601 format
  String ToLocalISO8601() const;

  /// Gets a ISO8601 string with milliseconds
  String ToLocalISO8601ms() const;

  /// Gets universal time in ISO8601 format
  String ToISO8601() const;

  /// Gets time with milliseconds in ISO8601 format
  String ToISO8601ms() const;

  /// Gets universal time with milliseconds in ISO8601 format
  String ToISO8601msTU() const;

  /// Gets a string in the international format
  ///
  /// @param bMillis with milliseconds if true
  ///
  String ToInter(bool bMillis=true) const;

  //@} Text methods

  //-----------------------------------------------------------------
  /// @name UNIX reference
  ///
  /// @note UNIX dates start from 1970/01/01. So theses methods
  /// are conveniance methods to set and get a number of seconds since
  /// the UNIX reference
  ///
  //@{
  
  /// Gets a (integer) number of second since 1970/01/01 0h
  long LongUnix() const;
  /// Sets the date from a (integer) number of seconds since 1970/01/01 0h
  void SetLongUnix(long lRefSec);
  /// Gets a (double) number of seconds since 1970/01/01 0h with microseconds precision
  double DoubleUnix() const;
  /// Sets the date from a (double) number seconds since 1970/01/01 0h with microseconds precision
  void SetDoubleUnix(double dRefSec);

  //@} UNIX reference

  //-----------------------------------------------------------------
  /// @name Compare operators
  //@{

  int operator ==(const Date& tm) const
    { return m_llDate == tm.m_llDate; }
  int operator >(const Date& tm) const
    { return m_llDate > tm.m_llDate; }
  int operator >=(const Date& tm) const
    { return m_llDate >= tm.m_llDate; }
  int operator <(const Date& tm) const
    { return m_llDate < tm.m_llDate; }
  int operator <=(const Date& tm)  const
    { return m_llDate <= tm.m_llDate; }
  int operator !=(const Date& tm) const
    { return m_llDate != tm.m_llDate; }

  //@} Compare operators

  //-----------------------------------------------------------------
  /// @name Static methods
  //@{

  /// Number of days for a given month
  static int NbDaysInMonth(int iMonth, int iYear);

  /// Number of days in a given year
  static int NbDaysInYear(int iYear);

  /// Month name
  static pcsz MonthName(int iMonth);

  /// Unix time
  static ulong UnixTime();

  //@} Static methods

};

/// Create synonyme for convenience
typedef Date Time;
typedef Date CDate;

//===========================================================================
/// CCurrentDate is CDate initialized with current date and time
//===========================================================================
class CurrentDate : public Date
{
public:
  /// Constructor
  ///
  /// @param bUT if true hte date is initialized with Universal Time instead of local time
  ///
  CurrentDate(bool bUT=false);
};

/// Create synonyme for convenience
typedef CurrentDate CurrentTime;
typedef CurrentDate CCurrentDate;

//===========================================================================
/// Date template evaluator
///
/// This class is a interface so it can't be directly instancied
/// @see TemplateString
//===========================================================================
class DateVariableEvaluator : public IVariableEvaluator
{
private:
  CDate m_tDate;
  bool  m_bFixedDate;
  bool  m_bUT;

public:
  /// Constructors
  /// @param bFixedDate if true the date used for evaluations is never change
  ///                   if false the evaluation is made using the current date
  /// @param bUT        Use universal time instead of local time
  DateVariableEvaluator(bool bFixedDate=true, bool bUT=false);
  DateVariableEvaluator(const CDate &aDate);
  
  // Set a new date
  void SetDate(const Date &aDate) { m_tDate = aDate; }

  /// Attempts to evaluate a variable contained in this set :
  /// __YEAR__, __DAY-OF-YEAR__, __MONTH__, __DAY__, __HOUR__, __MIN__, __SEC__
  ///
  /// @param pstrVar Variable to evaluate
  /// @return true if evaluation is done, or false
  virtual bool Evaluate(String *pstrVar);
};

typedef DateVariableEvaluator CDateVariableEvaluator;

} // namespace soleil

#endif
