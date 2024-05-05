/*
 * ExportableIfc.h
 *
 *  Created on: 18 Apr 2014
 *      Author: sven
 */

#ifndef EXPORTABLEIFC_H_
#define EXPORTABLEIFC_H_

#include <vector>

#include "LogData/LogEntry.h"

class QModelIndex;

/**
 * This interface can mark a model as exportable. A ExportableIfc model can
 * return a vector of the entries to export.
 */
class ExportableIfc
{
public:
  class ExportParameters
  {
  public:
    bool withoutFilter;
  };

  virtual ~ExportableIfc();

  /**
   * This generates the export list for this model by using the information.
   */
  virtual void generateExportList( std::vector<TconstSharedLogEntry>& entries
      , QModelIndex first, QModelIndex last
      , const ExportParameters& param ) const = 0;
};

#endif /* EXPORTABLEIFC_H_ */
