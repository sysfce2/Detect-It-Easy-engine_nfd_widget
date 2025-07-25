/* Copyright (c) 2020-2025 hors<horsicq@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef NFDWIDGETADVANCED_H
#define NFDWIDGETADVANCED_H

#include "specabstract.h"
#include "xdialogprocess.h"
#include "xshortcutswidget.h"
#include "scanitemmodel.h"

namespace Ui {
class NFDWidgetAdvanced;
}

class NFDWidgetAdvanced : public XShortcutsWidget {
    Q_OBJECT

public:
    explicit NFDWidgetAdvanced(QWidget *pParent = nullptr);
    ~NFDWidgetAdvanced();

    void setData(QIODevice *pDevice, bool bScan, XBinary::FT fileType);
    void setData(const QString &sFileName, const XScanEngine::SCAN_OPTIONS &scanOptions, bool bScan);
    // TODO Memory scan

    virtual void adjustView();
    void setGlobal(XShortcuts *pShortcuts, XOptions *pXOptions);
    void reloadData(bool bSaveSelection);

private slots:
    void on_toolButtonScan_clicked();
    void on_toolButtonSave_clicked();
    void on_comboBoxType_currentIndexChanged(int nIndex);
    void process();
    void on_tableViewHeur_customContextMenuRequested(const QPoint &pos);

protected:
    virtual void registerShortcuts(bool bState);

private:
    Ui::NFDWidgetAdvanced *ui;
    QIODevice *g_pDevice;
    QString g_sFileName;
    XBinary::FT g_fileType;
};

#endif  // NFDWIDGETADVANCED_H
