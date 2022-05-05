#include "job.h"
#include "jobresult.h"

Job::Job(QObject *parent)
    : QObject(parent),
      mAbort(false),
      pixelPositionY(0),
      moveOffset(0, 0),
      scaleFactor(0.0),
      areaSize(0, 0),
      iterationMax(1)
{
}

void Job::run()
{
    JobResult result(areaSize.width());
    result.areaSize = areaSize;
    result.pixelPositionY = pixelPositionY;
    result.moveOffset = moveOffset;
    result.scaleFactor = scaleFactor;

    double imageHalfWidth  = areaSize.width()  / 2.0;
    double imageHalfHeight = areaSize.height() / 2.0;

    for (int imageX = 0; imageX < areaSize.width(); ++imageX) {

        int iteration = 0;

        double x0 = (imageX - imageHalfWidth) * scaleFactor - moveOffset.x();
        double y0 = (pixelPositionY - imageHalfHeight) * scaleFactor - moveOffset.y();
        double x = 0.0;
        double y = 0.0;

        do {
            if (mAbort.load()) {
                return;
            }

            double nextX = (x*x) - (y*y) + x0;
            y = 2.0 * x * y + y0;
            x = nextX;
            iteration++;
        } while (iteration < iterationMax
                 && x*x + y*y < 4.0);

        result.values[imageX] = iteration;
    }

    emit jobCompleted(result);
}

void Job::setPixelPositionY(int value)
{
    if (value != pixelPositionY) {
        pixelPositionY = value;
    }
}

void Job::setMoveOffset(const QPointF &value)
{
    if (value != moveOffset) {
        moveOffset = value;
    }
}

void Job::setScaleFactor(double value)
{
    if (value != scaleFactor) {
        scaleFactor = value;
    }
}

void Job::setAreaSize(const QSize &value)
{
    if (value != areaSize) {
        areaSize = value;
    }
}

void Job::setIterationMax(int value)
{
    if (value != iterationMax) {
        iterationMax = value;
    }
}

void Job::abort()
{
    mAbort.store(true);
}
