#include "operation.h"
#include <QRegularExpression>

QString ReplaceOperation::perform(const QString &fileName) const
{
    QRegularExpression regex(m_pattern);
    if (regex.isValid()) {
        QString result = fileName;
        result.replace(regex, m_replacement);
        return result;
    }
    return fileName;
}

QString PrefixOperation::perform(const QString &fileName) const
{
    return m_prefix + fileName;
}

QString SuffixOperation::perform(const QString &fileName) const
{
    // Add suffix before extension
    // Note: dotIndex > 0 ensures we don't treat dotfiles (like .bashrc) as having extensions
    int dotIndex = fileName.lastIndexOf('.');
    if (dotIndex > 0) {
        return fileName.left(dotIndex) + m_suffix + fileName.mid(dotIndex);
    } else {
        return fileName + m_suffix;
    }
}

QString RemoveExtensionOperation::perform(const QString &fileName) const
{
    // Remove extension, but preserve dotfiles (like .bashrc)
    int dotIndex = fileName.lastIndexOf('.');
    if (dotIndex > 0) {
        return fileName.left(dotIndex);
    }
    return fileName;
}

QString ChangeExtensionOperation::perform(const QString &fileName) const
{
    // Change extension, but preserve dotfiles (like .bashrc)
    QString result = fileName;
    int dotIndex = result.lastIndexOf('.');
    if (dotIndex > 0) {
        result = result.left(dotIndex);
    }
    if (!m_newExtension.isEmpty()) {
        if (!m_newExtension.startsWith('.')) {
            result += '.';
        }
        result += m_newExtension;
    }
    return result;
}
