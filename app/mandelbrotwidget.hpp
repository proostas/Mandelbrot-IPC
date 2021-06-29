#ifndef MANDELBROTWIDGET_HPP
#define MANDELBROTWIDGET_HPP

#include <memory>

#include <QWidget>
#include <QPoint>
#include <QThread>
#include <QList>

#include "mandelbrotcalculator.hpp"

class MandelbrotWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MandelbrotWidget(QWidget *parent = nullptr);
    ~MandelbrotWidget();

public slots:
    void processJobResults(QList<JobResult> jobResults);

signals:
    void requestPicture(QSize areaSize, QPointF moveOffset, double scaleFactor, int iterationMax);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    QRgb generateColorFromIteration(int iteration);

private:
    MandelbrotCalculator m_mandelbrotCalculator;
    QThread m_threadCalculator;
    double m_scaleFactor;
    QPoint m_lastMouseMovePosition;
    QPointF m_moveOffset;
    QSize m_areaSize;
    int m_iterationMax;
    std::unique_ptr<QImage> m_image;

};

#endif // MANDELBROTWIDGET_HPP
