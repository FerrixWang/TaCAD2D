//! claas ShortcutManager
//! 

#include <QShortCut>
#include <QKeySequence>
#include <QAction>
#include <QHash>
#include <QList>
#include <QPair>

#include "Pres/Commands/RelayCommand.hxx"

/**
 * @brief Manages keyboard shortcuts for different scopes, using RelayCommand for actions.
 */
class ShortcutHandler : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Constructs a ShortcutHandler with an optional parent widget.
     * @param parent The parent QWidget for shortcuts (default: nullptr).
     */
    explicit ShortcutHandler(QWidget* parent = nullptr)
        : QObject(parent)
        , m_parent(parent)
    {}

    /**
     * @brief Returns the list of registered scopes.
     * @return QStringList of scope names.
     */
    QStringList GetScopes() const
    {
        return m_shortcutScopes.keys();
    }

    /**
     * @brief Adds a shortcut to a scope, associating a key sequence with a RelayCommand.
     * @param scope The scope identifier (e.g., "Application").
     * @param keySequence The key sequence (e.g., QKeySequence::Save).
     * @param command The RelayCommand to execute.
     * @param parameter Optional parameter for the command (as std::any).
     * @return True if added successfully, false if keySequence conflicts or invalid.
     */
    bool AddShortcut(const QString& scope, const QKeySequence& keySequence, RelayCommand* command, const std::any& parameter = {})
    {
        Q_ASSERT(command);
        if(!command || keySequence.isEmpty())
        {
            return false;
        }

        auto& shortcutList = m_shortcutScopes[scope];
        // Check for key sequence conflict
        for(const auto& shortcut : shortcutList)
        {
            if(shortcut.first == keySequence)
            {
                qWarning() << "Shortcut conflict detected in scope" << scope << "for key" << keySequence.toString();
                return false;
            }
        }

        shortcutList.append({keySequence, {command, parameter}});

        auto shortcut = QScopedPointer<QShortcut>(new QShortcut(keySequence, m_parent));
        connect(shortcut.data(), &QShortcut::activated, this, [=]() {
            if(command->canExecute(parameter))
            {
                command->execute(parameter);
                emit ShortcutActivated(scope, keySequence);
            }
        });
        m_shortcuts.append(shortcut.take());
        return true;
    }

    /**
     * @brief Adds multiple shortcuts to a scope.
     * @param scope The scope identifier.
     * @param shortcuts List of shortcuts (key sequence, command, parameter).
     */
    void AddShortcuts(const QString& scope, const QList<QPair<QKeySequence, QPair<RelayCommand*, std::any>>>& shortcuts)
    {
        for(const auto& shortcut : shortcuts)
        {
            AddShortcut(scope, shortcut.first, shortcut.second.first, shortcut.second.second);
        }
    }

    /**
     * @brief Triggers a shortcut in a scope if it exists and is executable.
     * @param scope The scope identifier.
     * @param keySequence The key sequence to trigger.
     * @return True if the shortcut was triggered, false otherwise.
     */
    bool KeyPressed(const QString& scope, const QKeySequence& keySequence) const
    {
        const auto& shortcuts = m_shortcutScopes[scope];

        for(const auto& shortcut : shortcuts)
        {
            if(shortcut.first == keySequence)
            {
                auto* command = shortcut.second.first;
                const auto& parameter = shortcut.second.second;
                if(command && command->canExecute(parameter))
                {
                    command->execute(parameter);
                    emit ShortcutActivated(scope, keySequence);
                    return true;
                }
                return false;
            }
        }
        return false;
    }

    /**
     * @brief Returns the shortcuts registered in a scope.
     * @param scope The scope identifier.
     * @return List of shortcuts (key sequence, command, parameter).
     */
    QList<QPair<QKeySequence, QPair<RelayCommand*, std::any>>> GetShortcutsForScope(const QString& scope) const
    {
        return m_shortcutScopes.value(scope, {});
    }

    /**
     * @brief Finds the key sequence for a given RelayCommand and parameter.
     * @param command The RelayCommand to query.
     * @param parameter The associated parameter.
     * @return The key sequence as a string, or empty if not found.
     */
    QString GetShortcutForCommand(const RelayCommand* command, const std::any& parameter) const
    {
        for(const auto& shortcuts : m_shortcutScopes)
        {
            for(const auto& shortcut : shortcuts)
            {
                if(shortcut.second.first == command && shortcut.second.second.type() == parameter.type())
                {
                    return shortcut.first.toString(QKeySequence::NativeText);
                }
            }
        }
        return QString();
    }

signals:
    /**
     * @brief Emitted when a shortcut is activated.
     * @param scope The scope of the shortcut.
     * @param keySequence The activated key sequence.
     */
    void ShortcutActivated(const QString& scope, const QKeySequence& keySequence) const;

private:
    using ShortcutData = QPair<QKeySequence, QPair<RelayCommand*, std::any>>;
    using ShortcutList = QList<ShortcutData>;

    QWidget* m_parent;
    QHash<QString, ShortcutList> m_shortcutScopes;
    QList<QShortcut*> m_shortcuts;
};
