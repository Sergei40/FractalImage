#include "mandelbrotwidget.h"
#include <QResizeEvent>
#include <QImage>
#include <QPainter>
#include <QtMath>

using namespace std;

const    int ITERATION_MAX    = 4000;
const double DEFAULT_SCALE    =    0.005;
const double DEFAULT_OFFSET_X =   -0.74364390249094747;
const double DEFAULT_OFFSET_Y =    0.13182589977450967;

MandelbrotWidget::MandelbrotWidget(QWidget *parent)
    : QWidget(parent),
      scaleFactor(DEFAULT_SCALE),
      moveOffset(DEFAULT_OFFSET_X, DEFAULT_OFFSET_Y),
      iterationMax(ITERATION_MAX)
{
    mandelbrotCalculator.moveToThread(&threadCalculator);

    connect(this, &MandelbrotWidget::requestPicture,
            &mandelbrotCalculator, &MandelbrotCalculator::generatePicture);

    connect(&mandelbrotCalculator, &MandelbrotCalculator::pictureLinesGenerated,
            this, &MandelbrotWidget::processJobResults);

    threadCalculator.start();
}

MandelbrotWidget::~MandelbrotWidget()
{
    threadCalculator.quit();
    threadCalculator.wait(1000);
    if (!threadCalculator.isFinished()) {
        threadCalculator.terminate();
    }
}

void MandelbrotWidget::processJobResults(QList<JobResult> jobResults)
{
    int yMin = height();
    int yMax = 0;

    for (JobResult &jobResult : jobResults) {
        if (image->size() != jobResult.areaSize) {
            continue;
        }
        int y = jobResult.pixelPositionY;
        QRgb *scanLine = reinterpret_cast<QRgb*>(image->scanLine(y));

        for (int x = 0; x < areaSize.width(); ++x) {
            scanLine[x] = generateColorFromIteration(jobResult.values[x]);
        }
        if (y < yMin) {
            yMin = y;
        }
        if (y > yMax) {
            yMax = y;
        }
    }
    repaint(0, yMin, width(), yMax);
}

void MandelbrotWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.save();

    QRect imageRect = event->region().boundingRect();
    painter.drawImage(imageRect, *image, imageRect);

    painter.setPen(Qt::white);

    painter.drawText(10, 20, tr("Size: %1 x %2").arg(image->width())
                                                .arg(image->height()));
    painter.drawText(10, 40, tr("Offset: %1 x %2").arg(moveOffset.x())
                                                  .arg(moveOffset.y()));
    painter.drawText(10, 60, tr("Scale: %1").arg(scaleFactor));
    painter.drawText(10, 80, tr("Max iteration: %1").arg(ITERATION_MAX));

    painter.restore();
}

void MandelbrotWidget::resizeEvent(QResizeEvent *event)
{
    areaSize = event->size();
    image = make_unique<QImage>(areaSize, QImage::Format_RGB32);
    image->fill(Qt::black);
    emit requestPicture(areaSize, moveOffset, scaleFactor, iterationMax);
}

void MandelbrotWidget::wheelEvent(QWheelEvent *event)
{
    int delta = event->delta();
    scaleFactor *= qPow(0.75, delta / 120.0);
    emit requestPicture(areaSize, moveOffset, scaleFactor, iterationMax);
}

void MandelbrotWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        lastMouseMovePosition = event->pos();
    }
}

void MandelbrotWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        QPointF offset = event->pos() - lastMouseMovePosition;
        lastMouseMovePosition = event->pos();
        offset.setY(offset.y());
        moveOffset += offset * scaleFactor;
        emit requestPicture(areaSize, moveOffset, scaleFactor, iterationMax);
    }
}

QRgb MandelbrotWidget::generateColorFromIteration(int iteration)
{
    if (iteration == iterationMax) {
        return qRgb(50, 50, 255);
    }
    return qRgb(0, 0, 255.0 * iteration / iterationMax);
}
