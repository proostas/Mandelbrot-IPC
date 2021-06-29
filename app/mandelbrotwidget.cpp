#include "mandelbrotwidget.hpp"

#include <QResizeEvent>
#include <QImage>
#include <QPainter>
#include <QtMath>

const int ITERATION_MAX = 4000;
const double DEFAULT_SCALE = 0.005;
const double DEFAULT_OFFSET_X = -0.74364390249094747;
const double DEFAULT_OFFSET_Y = 0.13182589977450967;

MandelbrotWidget::MandelbrotWidget(QWidget *parent)
    : QWidget(parent),
      m_mandelbrotCalculator(),
      m_threadCalculator(this),
      m_scaleFactor(DEFAULT_SCALE),
      m_lastMouseMovePosition(),
      m_moveOffset(DEFAULT_OFFSET_X, DEFAULT_OFFSET_Y),
      m_areaSize(),
      m_iterationMax(ITERATION_MAX)
{
    m_mandelbrotCalculator.moveToThread(&m_threadCalculator);
    connect(this, &MandelbrotWidget::requestPicture, &m_mandelbrotCalculator, &MandelbrotCalculator::generatePicture);
    connect(&m_mandelbrotCalculator, &MandelbrotCalculator::pictureLinesGenerated, this, &MandelbrotWidget::processJobResults);

    m_threadCalculator.start();
}

MandelbrotWidget::~MandelbrotWidget()
{
    m_threadCalculator.quit();
    m_threadCalculator.wait(1000);
    if (!m_threadCalculator.isFinished()) {
        m_threadCalculator.terminate();
    }
}

void MandelbrotWidget::processJobResults(QList<JobResult> jobResults)
{
    int yMin = height();
    int yMax = 0;

    for (JobResult& jobResult : jobResults) {
        if (m_image->size() != jobResult.areaSize)
            continue;

        int y = jobResult.pixelPositionY;
        QRgb* scanLine = reinterpret_cast<QRgb*>(m_image->scanLine(y));

        for (int x = 0; x < m_areaSize.width(); ++x) {
            scanLine[x] = generateColorFromIteration(jobResult.values[x]);
        }

        if (y < yMin) {
            yMin = y;
        }

        if (y > yMax) {
            yMax = y;
        }

        update(0, yMin, width(), yMax);
    }
}

void MandelbrotWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.save();

    QRect imageRect = event->region().boundingRect();
    painter.drawImage(imageRect, *m_image, imageRect);

    painter.setPen(Qt::white);

    painter.drawText(10, 20, QString("Size: %1 x %2").arg(m_image->width()).arg(m_image->height()));

    painter.drawText(10, 35, QString("Offset: %1 x %2").arg(m_moveOffset.x()).arg(m_moveOffset.y()));

    painter.drawText(10, 50, QString("Scale: %1").arg(m_scaleFactor));

    painter.drawText(10, 65, QString("Max iteration: %1").arg(ITERATION_MAX));

    painter.restore();
}

void MandelbrotWidget::resizeEvent(QResizeEvent* event)
{
    m_areaSize = event->size();

    m_image = std::make_unique<QImage>(m_areaSize, QImage::Format_RGB32);
    m_image->fill(Qt::black);

    emit requestPicture(m_areaSize, m_moveOffset, m_scaleFactor, m_iterationMax);
}

void MandelbrotWidget::wheelEvent(QWheelEvent* event)
{
    int delta = event->delta();
    m_scaleFactor *= qPow(0.75, delta / 120.0);
    emit requestPicture(m_areaSize, m_moveOffset, m_scaleFactor, m_iterationMax);
}

void MandelbrotWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        m_lastMouseMovePosition = event->pos();
    }
}

void MandelbrotWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        QPointF offset = m_lastMouseMovePosition - event->pos();
        m_lastMouseMovePosition = event->pos();
        offset.setY(-offset.y());
        m_moveOffset += offset * m_scaleFactor;
        emit requestPicture(m_areaSize, m_moveOffset, m_scaleFactor, m_iterationMax);
    }
}

QRgb MandelbrotWidget::generateColorFromIteration(int iteration)
{
    if (iteration == m_iterationMax)
        return qRgb(50, 50, 255);

    return qRgb(0, 0, (255.0 * iteration / m_iterationMax));
}
