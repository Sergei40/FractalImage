#ifndef MANDELBROTCALCULATOR_H
#define MANDELBROTCALCULATOR_H

#include <QObject>
#include <QSize>
#include <QPointF>
#include <QElapsedTimer>
#include <QList>

#include "jobresult.h"
class Job;

class MandelbrotCalculator : public QObject
{
    Q_OBJECT
public:
    explicit MandelbrotCalculator(QObject *parent = nullptr);
    void init(QSize imageSize);

signals:
    void pictureLinesGenerated(QList<JobResult> jobResults);
    void abortAllJobs();

public slots:
    void generatePicture(QSize areaSize, QPointF moveOffset,
                         double scaleFactor, int iterationMax);
    void process(JobResult jobResult);

private:
    Job* createJob(int pixelPositionY);
    void clearJobs();

private:
    QPointF moveOffset;
    double scaleFactor;
    QSize areaSize;
    int iterationMax;
    int receivedJobResults;
    QList<JobResult> jobResults;
    QElapsedTimer timer;
};

#endif // MANDELBROTCALCULATOR_H
