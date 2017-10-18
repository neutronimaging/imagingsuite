#ifndef __DATA_UTIL_HPP_GUARD__
#define __DATA_UTIL_HPP_GUARD__

/**
 * \file data_util.hpp
 */
#include <napi.h>
#include <string>

namespace nxsum {
  /**
   * \param operation The operation to check for support.
   *
   * \return True if the operation is supported.
   */
  bool operationValid(const std::string &operation);

  /**
   * This will open the file to the supplied path and perform the
   * requested operation. Note that this leaves the filehandle
   * pointing at the node specified in the path.
   *
   * \param handle Handle for the NeXus files.
   * \param path Location in the NeXus file to look for the data at.
   * \param operation Operation to perform on the data. If empty this
   * returns the data itself, unchanged.
   *
   * \return String version of the data after the operation is performed.
   */
  std::string readAsString(NXhandle handle, const std::string &path,
                           const std::string &operation);

  /**
   * \param The integer type to convert to a string.
   *
   * \return String representation of the supplied type.
   */
  std::string nxtypeAsString(const int type);
}

#endif
