#ifndef JOBREQUEST_HPP
#define JOBREQUEST_HPP

#include <QSize>
#include <QPointF>
#include <QMetaType>
#include <QDataStream>

struct JobRequest
{
    int pixelPosistionY;
    QPointF moveOffset;
    double scaleFactor;
    QSize areaSize;
    int iterationMax;
};

inline QDataStream& operator<<(QDataStream& out, JobRequest const& jobRequest)
{
    out << jobRequest.pixelPosistionY
        << jobRequest.moveOffset
        << jobRequest.scaleFactor
        << jobRequest.areaSize
        << jobRequest.iterationMax;
    return out;
}

inline QDataStream& operator>>(QDataStream& in, JobRequest& jobRequest)
{
    in >> jobRequest.pixelPosistionY;
    in >> jobRequest.moveOffset;
    in >> jobRequest.scaleFactor;
    in >> jobRequest.areaSize;
    in >> jobRequest.iterationMax;
    return in;
}

// for QVariant integration; used indirectly through QDataStream
Q_DECLARE_METATYPE(JobRequest)

#endif // JOBREQUEST_HPP
