// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "data.h"

#include <QApplication>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QSlider>
#include <QTextEdit>
#include <QScreen>
#include <QPainter>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QWidget *widget = new QWidget();
    QHBoxLayout *hLayout = new QHBoxLayout(widget);
    QVBoxLayout *vLayout = new QVBoxLayout();

    Q3DSurface *surface = new Q3DSurface();
    Q3DScatter *scatter = new Q3DScatter();
    Q3DBars *bars = new Q3DBars();

    QSize screenSize = surface->screen()->size();

    surface->setMinimumSize(QSize(screenSize.height() / 1.2, screenSize.height() / 1.2));
    surface->setMaximumSize(screenSize);
    surface->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    surface->setFocusPolicy(Qt::StrongFocus);
    surface->setResizeMode(QQuickWidget::SizeRootObjectToView);

    scatter->setMinimumSize(QSize(screenSize.height() / 1.2, screenSize.height() / 1.2));
    scatter->setMaximumSize(screenSize);
    scatter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scatter->setFocusPolicy(Qt::StrongFocus);
    scatter->setVisible(false);
    scatter->setResizeMode(QQuickWidget::SizeRootObjectToView);

    bars->setMinimumSize(QSize(screenSize.height() / 1.2, screenSize.height() / 1.2));
    bars->setMaximumSize(screenSize);
    bars->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    bars->setFocusPolicy(Qt::StrongFocus);
    bars->setVisible(false);
    bars->setResizeMode(QQuickWidget::SizeRootObjectToView);

    widget->setWindowTitle(QStringLiteral("Test switching graphs on the fly"));

    hLayout->addWidget(surface, 1);
    hLayout->addWidget(scatter, 1);
    hLayout->addWidget(bars, 1);
    hLayout->addLayout(vLayout);

    QPushButton *startButton = new QPushButton(widget);
    startButton->setText(QStringLiteral("Start"));

    QPushButton *stopButton = new QPushButton(widget);
    stopButton->setText(QStringLiteral("Stop"));

    QComboBox *resolutionBox = new QComboBox(widget);
    resolutionBox->addItem(QStringLiteral("Low"));
    resolutionBox->addItem(QStringLiteral("Medium"));
    resolutionBox->addItem(QStringLiteral("High"));
    resolutionBox->addItem(QStringLiteral("Max")); // Comment this out if demo machine is low-perf
    resolutionBox->setCurrentIndex(0);

    QComboBox *modeBox = new QComboBox(widget);
    modeBox->addItem(QStringLiteral("Surface Plot"));
    modeBox->addItem(QStringLiteral("Scatter Chart"));
    modeBox->addItem(QStringLiteral("Bar Chart"));
    modeBox->setCurrentIndex(0);

    QLinearGradient gradientOne(0, 0, 200, 1);
    gradientOne.setColorAt(0.0, Qt::black);
    gradientOne.setColorAt(0.33, Qt::blue);
    gradientOne.setColorAt(0.67, Qt::red);
    gradientOne.setColorAt(1.0, Qt::yellow);

    QPixmap pm(200, 24);
    QPainter pmp(&pm);
    pmp.setBrush(QBrush(gradientOne));
    pmp.setPen(Qt::NoPen);
    pmp.drawRect(0, 0, 200, 24);

    QPushButton *gradientOneButton = new QPushButton(widget);
    gradientOneButton->setIcon(QIcon(pm));
    gradientOneButton->setIconSize(QSize(200, 24));
    gradientOneButton->setToolTip(QStringLiteral("Colors: Thermal Imitation"));

    QLinearGradient gradientTwo(0, 0, 200, 1);
    gradientTwo.setColorAt(0.0, Qt::white);
    gradientTwo.setColorAt(0.8, Qt::red);
    gradientTwo.setColorAt(1.0, Qt::green);

    pmp.setBrush(QBrush(gradientTwo));
    pmp.setPen(Qt::NoPen);
    pmp.drawRect(0, 0, 200, 24);

    QPushButton *gradientTwoButton = new QPushButton(widget);
    gradientTwoButton->setIcon(QIcon(pm));
    gradientTwoButton->setIconSize(QSize(200, 24));
    gradientTwoButton->setToolTip(QStringLiteral("Colors: Highlight Foreground"));

    QTextEdit *status = new QTextEdit(QStringLiteral("<b>Ready</b><br>"), widget);
    status->setReadOnly(true);

    vLayout->addWidget(startButton);
    vLayout->addWidget(stopButton);
    vLayout->addWidget(new QLabel(QStringLiteral("Change resolution")));
    vLayout->addWidget(resolutionBox);
    vLayout->addWidget(new QLabel(QStringLiteral("Change graphs type")));
    vLayout->addWidget(modeBox);
    vLayout->addWidget(new QLabel(QStringLiteral("Change color scheme")));
    vLayout->addWidget(gradientOneButton);
    vLayout->addWidget(gradientTwoButton);
    vLayout->addWidget(status, 1, Qt::AlignBottom);

    widget->show();

    Data datagen(surface, scatter, bars, status, widget);
    ContainerChanger changer(surface, scatter, bars, gradientOneButton, gradientTwoButton);

    QObject::connect(startButton, &QPushButton::clicked, &datagen, &Data::start);
    QObject::connect(stopButton, &QPushButton::clicked, &datagen, &Data::stop);
    QObject::connect(resolutionBox, SIGNAL(activated(int)), &datagen, SLOT(setResolution(int)));
    QObject::connect(modeBox, SIGNAL(activated(int)), &changer, SLOT(changeContainer(int)));
    QObject::connect(modeBox, SIGNAL(activated(int)), &datagen, SLOT(changeMode(int)));
    QObject::connect(status, &QTextEdit::textChanged, &datagen, &Data::scrollDown);
    QObject::connect(gradientOneButton, &QPushButton::clicked, &datagen,
                     &Data::useGradientOne);
    QObject::connect(gradientTwoButton, &QPushButton::clicked, &datagen,
                     &Data::useGradientTwo);

    return app.exec();
}
