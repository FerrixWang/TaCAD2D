// Copyright [2025] TaCAD

#include "Iact/HudElements/Cursors.h"

inline QCursor Cursors::wait() { return Qt::WaitCursor; }

inline QCursor Cursors::move() { return getOrCreate("Move"); }

inline QCursor Cursors::Rotate() { return getOrCreate("Rotate"); }

inline QCursor Cursors::selectShape() { return getOrCreate("SelectShape"); }

inline QCursor Cursors::selectVertex() { return getOrCreate("SelectVertex"); }

inline QCursor Cursors::selectEdge() { return getOrCreate("SelectEdge"); }

inline QCursor Cursors::selectWire() { return getOrCreate("SelectWire"); }

inline QCursor Cursors::selectFace() { return getOrCreate("SelectFace"); }

inline QCursor Cursors::setPoint() { return getOrCreate("SetPoint"); }

inline QCursor Cursors::setRadius() { return getOrCreate("SetRadius"); }

inline QCursor Cursors::setHeight() { return getOrCreate("SetHeight"); }

inline QCursor Cursors::WorkingPlane() { return getOrCreate("_WorkingPlane"); }

inline QCursor Cursors::plus() { return getOrCreate("Plus"); }

inline QCursor Cursors::minus() { return getOrCreate("Minus"); }

QMap<QString, QCursor> Cursors::s_cursors;

QCursor Cursors::getOrCreate(const QString& cursorId)
{
    if (!s_cursors.contains(cursorId)) {

        QString path = ":/Cursors/" + cursorId + ".cur";
        QFile file(path);

        if (!file.exists()) {
            qWarning() << "Cursor file not found:" << path;
            return Qt::ArrowCursor;
        }

        QPixmap pixmap(path);
        QCursor cursor(pixmap);
        s_cursors.insert(cursorId, cursor);
    }

    return s_cursors.value(cursorId, Qt::ArrowCursor);
}
