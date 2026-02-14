#include "operation.h"
#include <QRegularExpression>

QString ReplaceOperation::perform(const QString &fileName) const
{
    QRegularExpression regex(m_pattern);
    if (regex.isValid()) {
        // Separate basename from extension
        int dotIndex = fileName.lastIndexOf('.');
        if (dotIndex > 0) {
            // Has extension - only replace in basename
            QString baseName = fileName.left(dotIndex);
            QString extension = fileName.mid(dotIndex);
            baseName.replace(regex, m_replacement);
            return baseName + extension;
        } else {
            // No extension - replace entire filename
            QString result = fileName;
            result.replace(regex, m_replacement);
            return result;
        }
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

QString InsertOperation::perform(const QString &fileName) const
{
    // Separate basename from extension
    int dotIndex = fileName.lastIndexOf('.');
    QString baseName;
    QString extension;
    
    if (dotIndex > 0) {
        // Has extension - only insert in basename
        baseName = fileName.left(dotIndex);
        extension = fileName.mid(dotIndex);
    } else {
        // No extension - use entire filename as basename
        baseName = fileName;
        extension = "";
    }
    
    // Insert text at the specified position within the basename
    // If position is negative or beyond the basename length, handle gracefully
    int pos = m_position;
    if (pos < 0) {
        pos = 0;
    }
    if (pos > baseName.length()) {
        pos = baseName.length();
    }
    
    baseName.insert(pos, m_text);
    return baseName + extension;
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
