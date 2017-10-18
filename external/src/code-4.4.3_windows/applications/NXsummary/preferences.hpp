/**
 * \file preferences.hpp
 */
#ifndef __PREFERENCES_HPP_GUARD__
#define __PREFERENCES_HPP_GUARD__

#include <string>
#include <vector>
#include "nxsummary.hpp"

namespace nxsum {
  /**
   * \param filename The name of the file to check.
   *
   * \return True if the file can be read.
   */
  bool canRead(const std::string &filename);

  /**
   * Load the preferences from the supplied file. This will search in
   * other locations if the supplied filename is not readable.
   *
   * \param filename The name of the file to load the preferences from.
   * \param preferences Parameter to put the loaded preferences in.
   */
  void loadPreferences(const std::string &filename,
                       std::vector<Item> &preferences);

  /**
   * Write out the supplied preferences.
   *
   * \param filename The name of the file to write the preferences into.
   * \param preferences List of preferences to write out.
   */
  void writePreferences(const std::string &filename,
                        const std::vector<Item> &preferences);

  /**
   * Get a preference with the supplied label. The label does not need
   * to be an exact match, just unique identifier (case insensitive).
   *
   * \param label Preference to look for.
   * \param preferences List of preferences to search inside of.
   */
  Item getPreference(const std::string &label,
                     const std::vector<Item> &preferences);
}
#endif
