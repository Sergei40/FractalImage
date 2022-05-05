#include "mandelbrotcalculator.h"
#include <QDebug>
#include <QThreadPool>
#include "job.h"

const int JOB_RESULT_THRESHOLD = 10;

MandelbrotCalculator::MandelbrotCalculator(QObject *parent)
    : QObject(parent),
      moveOffset(0.0, 0.0),
      scaleFactor(0.005),
      areaSize(800, 600),
      iterationMax(10),
      receivedJobResults(0)
{
}

void MandelbrotCalculator::generatePicture(QSize areaSize, QPointF moveOffset, double scaleFactor,
                                           int iterationMax)
{
    if (areaSize.isEmpty()) {
        return;
    }

    timer.start();
    clearJobs();

    this->areaSize = areaSize;
    this->moveOffset = moveOffset;
    this->scaleFactor = scaleFactor;
    this->iterationMax = iterationMax;

    for (int pixelPositionY = 0; pixelPositionY < areaSize.height(); ++pixelPositionY) {
        QThreadPool::globalInstance()->start(createJob(pixelPositionY));
    }
}

void MandelbrotCalculator::process(JobResult jobResult)
{
    if (jobResult.areaSize != areaSize || jobResult.moveOffset != moveOffset ||
        jobResult.areaSize != areaSize) {
        return;
    }

    receivedJobResults++;
    jobResults.append(jobResult);

    if (jobResults.size() >= JOB_RESULT_THRESHOLD || receivedJobResults == areaSize.height()) {
        emit pictureLinesGenerated(jobResults);
        jobResults.clear();
    }

    if (receivedJobResults == areaSize.height()) {
        qDebug() << "Generated in " << timer.elapsed() << " ms";
    }
}

Job *MandelbrotCalculator::createJob(int pixelPositionY)
{
    Job *job = new Job();
    job->setPixelPositionY(pixelPositionY);
    job->setMoveOffset(moveOffset);
    job->setScaleFactor(scaleFactor);
    job->setAreaSize(areaSize);
    job->setIterationMax(iterationMax);

    connect(this, &MandelbrotCalculator::abortAllJobs, job, &Job::abort);
    connect(job, &Job::jobCompleted, this, &MandelbrotCalculator::process);

    return job;
}

void MandelbrotCalculator::clearJobs()
{
    receivedJobResults = 0;
    emit abortAllJobs();
    QThreadPool::globalInstance()->clear();
}
