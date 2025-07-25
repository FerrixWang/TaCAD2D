// ResourceUtils.h
#ifndef _ResourceUtils_Header_File_
#define _ResourceUtils_Header_File_

// Qt includes
#include <QFile>
#include <QIcon>
#include <QImage>
#include <QString>

class ResourceUtils
{
public:
    static constexpr const char* ICON_PATH_PREFIX = "://Icons/";
    static constexpr const char* MARKER_PATH_PREFIX = "://Marker/";

    static QString iconPath(const QString& iconName)
    {
        return QString("%1%2.svg").arg(ICON_PATH_PREFIX, iconName);
    }

    static QString markerPath(const QString& iconName)
    {
        return QString("%1%2.svg").arg(MARKER_PATH_PREFIX, iconName);
    }

    static QIcon icon(const QString& iconName)
    {
        return QIcon(iconPath(iconName));
    }

    static QString GetResourceUri(const QString& filename)
    {
        return assemblyName().append(filename);
    }

    static QImage readBitmapFromResource(const QString& filename)
    {
        return QImage(GetResourceUri(filename));
    }

public:
    static bool isResourcePathValid(const QString& resourcePath)
    {
        QFile file(resourcePath);
        return file.exists();
    }

private:
    static QString assemblyName()
    {
        return ":/";
    };
};

#endif // _ResourceUtils_Header_File_
