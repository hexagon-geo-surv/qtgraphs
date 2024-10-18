// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
#include "graphprinter.h"
#include <QtGui/qtransform.h>
#include <QtPrintSupport>

GraphPrinter::GraphPrinter(QObject *parent)
    : QObject(parent)
{}

GraphPrinter::~GraphPrinter() {}

void GraphPrinter::generatePDF(const QUrl &path, const QImage &image)
{
    //! [0]
    const QFile file = QFile(path.toLocalFile() + QStringLiteral("/graph.pdf"));

    QPdfWriter writer(file.fileName());
    writer.setResolution(90);
    writer.setTitle("Graph");
    writer.setPageSize(QPageSize(image.size()));
    writer.setPageMargins(QMarginsF(0, 0, 0, 0));
    writer.newPage();
    //! [0]

    //! [1]
    QPainter painter(&writer);
    const QImage finalImage = image.scaled(painter.viewport().size(), Qt::KeepAspectRatio);
    painter.setRenderHint(QPainter::LosslessImageRendering);
    painter.drawImage(finalImage.rect(), finalImage);
    //! [1]

    qInfo("printed PDF to %ls", qUtf16Printable(file.fileName()));
}

//! [2]
void GraphPrinter::print(const QImage &image, const QString printerName)
{
    QPrinterInfo printInfo = QPrinterInfo::printerInfo(printerName);
    if (printInfo.isNull()) {
        qWarning("%ls is not a valid printer", qUtf16Printable(printerName));
        return;
    }

    QPrinter printer(printInfo, QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::NativeFormat);

    QPainter painter(&printer);
    const QImage finalImage = image.scaled(painter.viewport().size(), Qt::KeepAspectRatio);
    painter.setRenderHint(QPainter::LosslessImageRendering);
    painter.drawImage(finalImage.rect(), finalImage);

    qInfo("printed image with %ls", qUtf16Printable(printerName));
}
//! [2]

//! [3]
QStringList GraphPrinter::getPrinters()
{
    return QPrinterInfo::availablePrinterNames();
}
//! [3]
