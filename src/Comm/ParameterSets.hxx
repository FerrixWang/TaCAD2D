// OverridableParameterSet.h
#ifndef _ParameterSet_Header_File_
#define _ParameterSet_Header_File_

#include <QObject>
#include <QSettings>
#include <QMap>
#include <QString>
#include <QApplication>

class OverridableParameterSet : public QObject
{
    Q_OBJECT
public:
    explicit OverridableParameterSet(const QString& group, QObject* parent = nullptr)
        : QObject(parent), _group(group)
    {}

    bool HasOverriddenParameters() const { return !_overriddenValues.isEmpty(); }

    template<typename T>
    T GetValue(const QString& key) const
    {
        QVariant value;
        if(_overriddenValues.contains(key))
        {
            return _overriddenValues[key].value<T>();
        }
        if(_defaultValues.contains(key))
        {
            return _defaultValues[key].value<T>();
        }
        qWarning() << "No default value for key:" << key;
        return T();
    }

    template<typename T>
    void SetValue(const QString& key, const T& value)
    {
        _overriddenValues[key] = value;
        emit ParameterChanged(key);
    }

    void ResetValue(const QString& key)
    {
        if(_overriddenValues.remove(key))
        {
            emit ParameterChanged(key);
        }
    }

    const QMap<QString, QVariant>& OverriddenValues() const { return _overriddenValues; }

signals:
    void ParameterChanged(const QString& key);

protected:
    void SetDefaultValue(const QString& key, const QVariant& value)
    {
        _defaultValues[key] = value;
    }

private:
    QString _group;
    QMap<QString, QVariant> _overriddenValues;
    QMap<QString, QVariant> _defaultValues;

    friend class ParameterSets;
};

class ParameterSets : public QSettings
{
    Q_OBJECT
public:
    explicit ParameterSets(const std::string& name, QObject* parent = nullptr)
        : QSettings(QSettings::IniFormat, QSettings::UserScope, qApp->applicationName(), QString::fromStdString(name), parent)
    {}

    template<typename T>
    T* Get()
    {
        static_assert(std::is_base_of_v<OverridableParameterSet, T>, "T must inherit from OverridableParameterSet");
        auto typeName = QString(T::staticMetaObject.className());
        if(_parameterSets.contains(typeName))
        {
            return qobject_cast<T*>(_parameterSets[typeName]);
        }
        T* newSet = new T(typeName, this);
        _parameterSets[typeName] = newSet;
        Read(newSet);
        return newSet;
    }

    void Write(OverridableParameterSet* set)
    {
        beginGroup(set->_group);
        remove(""); // Çå³ýÏÖÓÐ¼ü
        for(auto it = set->_overriddenValues.constBegin(); it != set->_overriddenValues.constEnd(); ++it)
        {
            setValue(it.key(), it.value());
        }
        endGroup();
        sync();
    }

    void Read(OverridableParameterSet* set)
    {
        beginGroup(set->_group);
        for(const QString& key : allKeys())
        {
            set->_overriddenValues[key] = value(key);
        }
        endGroup();
    }

private:
    QMap<QString, OverridableParameterSet*> _parameterSets;
};

#endif  // _ParameterSet_Header_File_
