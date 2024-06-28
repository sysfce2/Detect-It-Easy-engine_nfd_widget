/* Copyright (c) 2020-2024 hors<horsicq@gmail.com>
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
#include "nfd_widget.h"

#include "ui_nfd_widget.h"

NFD_Widget::NFD_Widget(QWidget *pParent) : XShortcutsWidget(pParent), ui(new Ui::NFD_Widget)
{
    ui->setupUi(this);

    g_pdStruct = XBinary::createPdStruct();

    connect(&watcher, SIGNAL(finished()), this, SLOT(on_scanFinished()));

    ui->checkBoxDeepScan->setChecked(true);
    ui->checkBoxHeuristicScan->setChecked(true);
    ui->checkBoxRecursiveScan->setChecked(true);
    ui->checkBoxAllTypesScan->setChecked(false);

    g_pTimer = new QTimer(this);
    connect(g_pTimer, SIGNAL(timeout()), this, SLOT(timerSlot()));

    clear();
}

NFD_Widget::~NFD_Widget()
{
    if (g_bProcess) {
        stop();
        watcher.waitForFinished();
    }

    delete ui;
}

void NFD_Widget::setData(const QString &sFileName, bool bScan, XBinary::FT fileType)
{
    clear();

    this->sFileName = sFileName;
    this->g_fileType = fileType;
    g_scanType = ST_FILE;

    if (bScan) {
        process();
    }
}

void NFD_Widget::setGlobal(XShortcuts *pShortcuts, XOptions *pXOptions)
{
    ui->checkBoxAllTypesScan->setChecked(pXOptions->getValue(XOptions::ID_SCAN_ALLTYPES).toBool());
    ui->checkBoxDeepScan->setChecked(pXOptions->getValue(XOptions::ID_SCAN_DEEP).toBool());
    ui->checkBoxRecursiveScan->setChecked(pXOptions->getValue(XOptions::ID_SCAN_RECURSIVE).toBool());
    ui->checkBoxHeuristicScan->setChecked(pXOptions->getValue(XOptions::ID_SCAN_HEURISTIC).toBool());
    ui->checkBoxVerbose->setChecked(pXOptions->getValue(XOptions::ID_SCAN_VERBOSE).toBool());

    XShortcutsWidget::setGlobal(pShortcuts, pXOptions);
}

void NFD_Widget::adjustView()
{
}

void NFD_Widget::clear()
{
    g_scanType = ST_UNKNOWN;
    g_bProcess = false;
    g_scanOptions = {};
    g_scanResult = {};
}

void NFD_Widget::process()
{
    if (!g_bProcess) {
        g_bProcess = true;
        enableControls(false);

        ui->pushButtonNfdScanStart->setText(tr("Stop"));

        g_scanOptions.bIsRecursiveScan = ui->checkBoxRecursiveScan->isChecked();
        g_scanOptions.bIsDeepScan = ui->checkBoxDeepScan->isChecked();
        g_scanOptions.bIsHeuristicScan = ui->checkBoxHeuristicScan->isChecked();
        g_scanOptions.bIsVerbose = ui->checkBoxVerbose->isChecked();
        g_scanOptions.bAllTypesScan = ui->checkBoxAllTypesScan->isChecked();
        g_scanOptions.fileType = g_fileType;
        //    scanOptions.bDebug=true;

        getGlobalOptions()->setValue(XOptions::ID_SCAN_ALLTYPES, g_scanOptions.bAllTypesScan);
        getGlobalOptions()->setValue(XOptions::ID_SCAN_DEEP, g_scanOptions.bIsDeepScan);
        getGlobalOptions()->setValue(XOptions::ID_SCAN_RECURSIVE, g_scanOptions.bIsRecursiveScan);
        getGlobalOptions()->setValue(XOptions::ID_SCAN_HEURISTIC, g_scanOptions.bIsHeuristicScan);
        getGlobalOptions()->setValue(XOptions::ID_SCAN_VERBOSE, g_scanOptions.bIsVerbose);

        g_pTimer->start(200);  // TODO const

        ui->progressBar0->hide();
        ui->progressBar1->hide();
        ui->progressBar2->hide();
        ui->progressBar3->hide();
        ui->progressBar4->hide();

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        QFuture<void> future = QtConcurrent::run(&NFD_Widget::scan, this);
#else
        QFuture<void> future = QtConcurrent::run(this, &NFD_Widget::scan);
#endif

        watcher.setFuture(future);
    } else {
        ui->pushButtonNfdScanStart->setEnabled(false);
        stop();
        watcher.waitForFinished();
        ui->pushButtonNfdScanStart->setText(tr("Scan"));
        enableControls(true);
    }
}

void NFD_Widget::scan()
{
    if (g_scanType != ST_UNKNOWN) {
        if (g_scanType == ST_FILE) {
            emit scanStarted();

            g_pdStruct = XBinary::createPdStruct();

            g_staticScan.setData(sFileName, &g_scanOptions, &g_scanResult, &g_pdStruct);
            g_staticScan.process();

            emit scanFinished();
        }
    }
}

void NFD_Widget::stop()
{
    g_pdStruct.bIsStop = true;
}

void NFD_Widget::on_scanFinished()
{
    enableControls(true);

    g_pTimer->stop();

    QAbstractItemModel *pOldModel = ui->treeViewResult->model();

    QList<XBinary::SCANSTRUCT> _listRecords = SpecAbstract::convert(&(g_scanResult.listRecords));

    ScanItemModel *pModel = new ScanItemModel(&_listRecords, 1, getGlobalOptions()->getValue(XOptions::ID_SCAN_HIGHLIGHT).toBool());
    ui->treeViewResult->setModel(pModel);
    ui->treeViewResult->expandAll();

    deleteOldAbstractModel(&pOldModel);

    ui->lineEditElapsedTime->setText(QString("%1 %2").arg(QString::number(g_scanResult.nScanTime), tr("msec")));

    g_bProcess = false;

    ui->pushButtonNfdScanStart->setEnabled(true);
    ui->pushButtonNfdScanStart->setText(tr("Scan"));
}

void NFD_Widget::on_pushButtonNfdExtraInformation_clicked()
{
    ScanItemModel *pModel = static_cast<ScanItemModel *>(ui->treeViewResult->model());

    if (pModel) {
        DialogTextInfo dialogInfo(this);

        dialogInfo.setText(pModel->toString(XBinary::FORMATTYPE_TEXT));

        dialogInfo.exec();
    }
}

void NFD_Widget::enableControls(bool bState)
{
    if (!bState) {
        QAbstractItemModel *pOldModel = ui->treeViewResult->model();
        ui->treeViewResult->setModel(0);

        deleteOldAbstractModel(&pOldModel);
    }

    ui->treeViewResult->setEnabled(bState);
    ui->checkBoxDeepScan->setEnabled(bState);
    ui->checkBoxHeuristicScan->setEnabled(bState);
    ui->checkBoxRecursiveScan->setEnabled(bState);
    ui->checkBoxVerbose->setEnabled(bState);
    ui->checkBoxAllTypesScan->setEnabled(bState);
    ui->pushButtonNfdDirectoryScan->setEnabled(bState);
    ui->pushButtonNfdExtraInformation->setEnabled(bState);
    ui->pushButtonNfdInfo->setEnabled(bState);
    ui->lineEditElapsedTime->setEnabled(bState);

    if (bState) {
        ui->stackedWidgetNfdScan->setCurrentIndex(0);
    } else {
        ui->stackedWidgetNfdScan->setCurrentIndex(1);
    }
}

void NFD_Widget::on_pushButtonNfdDirectoryScan_clicked()
{
    DialogNFDScanDirectory dds(this, QFileInfo(sFileName).absolutePath());

    dds.exec();
}

void NFD_Widget::registerShortcuts(bool bState)
{
    Q_UNUSED(bState)
}

void NFD_Widget::on_pushButtonNfdInfo_clicked()
{
    if (!g_scanOptions.bHandleInfo) {
        DialogNFDWidgetAdvanced dnwa(this);
        dnwa.setGlobal(getShortcuts(), getGlobalOptions());

        dnwa.setData(sFileName, g_scanOptions, true);

        dnwa.exec();
    } else {
        emit showInfo();
    }
}

void NFD_Widget::on_pushButtonNfdScanStart_clicked()
{
    ui->pushButtonNfdScanStart->setEnabled(false);
    process();
    ui->pushButtonNfdScanStart->setEnabled(true);
}

void NFD_Widget::on_pushButtonNfdScanStop_clicked()
{
    ui->pushButtonNfdScanStop->setEnabled(false);
    process();
    ui->pushButtonNfdScanStop->setEnabled(true);
}

void NFD_Widget::timerSlot()
{
    XFormats::setProgressBar(ui->progressBar0, g_pdStruct._pdRecord[0]);
    XFormats::setProgressBar(ui->progressBar1, g_pdStruct._pdRecord[1]);
    XFormats::setProgressBar(ui->progressBar2, g_pdStruct._pdRecord[2]);
    XFormats::setProgressBar(ui->progressBar3, g_pdStruct._pdRecord[3]);
    XFormats::setProgressBar(ui->progressBar4, g_pdStruct._pdRecord[4]);
}
