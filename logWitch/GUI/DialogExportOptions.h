/*
 * DialogExportOptions.h
 *
 *  Created on: 26 Oct 2013
 *      Author: sven
 */

#ifndef DIALOGEXPORTOPTIONS_H_
#define DIALOGEXPORTOPTIONS_H_

#include <QDialog>

#include "ui_DialogExportOptions.h"

class DialogExportOptions: public QDialog
{
    Q_OBJECT
public:
    DialogExportOptions(QWidget * parent = 0);

    enum ExportOption { Selection, SelectionWithoutFilter, All, None };

    /**
     * Sets the export options to the dialog as a default.
     */
    void setExportOption( ExportOption o );

    /**
     * Retrieve the current selected export options.
     */
    ExportOption getExportOption() const;

private:
    Ui::DialogExportOptions ui{};
};

#endif /* DIALOGEXPORTOPTIONS_H_ */
