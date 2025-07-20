#ifndef IACT_HUD_ELEMENTS_CURSORS_H_
#define IACT_HUD_ELEMENTS_CURSORS_H_

#include <QCursor>
#include <QMap>
#include <QFile>
#include <QPixmap>
#include <QString>
#include <QDebug>

class Cursors {
 public:
    static QCursor wait();
    static QCursor move();
    static QCursor Rotate();
    static QCursor selectShape();
    static QCursor selectVertex();
    static QCursor selectEdge();
    static QCursor selectWire();
    static QCursor selectFace();
    static QCursor setPoint();
    static QCursor setRadius();
    static QCursor setHeight();
    static QCursor WorkingPlane();
    static QCursor plus();
    static QCursor minus();

 private:
    static QMap<QString, QCursor> s_cursors;

    static QCursor getOrCreate(const QString& cursor_id);
};

#endif  // IACT_HUD_ELEMENTS_CURSORS_H_
