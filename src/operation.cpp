#include "operation.h"
#include <QRegularExpression>

QString Operation::replaceTags(const QString &text, int fileIndex)
{
    QString result = text;
    
    // Pattern to match tags like <0:0>, <00:5>, <000:14>, or <0>
    // Format: <(0+)(:(\\d+))?>
    QRegularExpression tagPattern(R"(<(0+)(?::(\d+))?>)");
    
    QRegularExpressionMatchIterator iter = tagPattern.globalMatch(text);
    
    // Process matches in reverse order to maintain correct positions
    QList<QRegularExpressionMatch> matches;
    while (iter.hasNext()) {
        matches.prepend(iter.next());
    }
    
    for (const QRegularExpressionMatch &match : matches) {
        QString fullMatch = match.captured(0);
        QString zeros = match.captured(1);
        QString startNumStr = match.captured(2);
        
        // Determine minimum width from number of zeros
        int minWidth = zeros.length();
        
        // Determine starting number (default is 1 if not specified)
        int startNum = startNumStr.isEmpty() ? 1 : startNumStr.toInt();
        
        // Calculate the actual number for this file
        int number = startNum + fileIndex;
        
        // Format the number with leading zeros
        QString formattedNumber = QString("%1").arg(number, minWidth, 10, QChar('0'));
        
        // Replace this occurrence
        result.replace(match.capturedStart(0), match.capturedLength(0), formattedNumber);
    }
    
    return result;
}

QString ReplaceOperation::perform(const QString &fileName, int fileIndex) const
{
    QRegularExpression regex(m_pattern);
    if (regex.isValid()) {
        // First replace tags in the replacement string
        QString replacementWithTags = replaceTags(m_replacement, fileIndex);
        
        // Separate basename from extension
        int dotIndex = fileName.lastIndexOf('.');
        if (dotIndex > 0) {
            // Has extension - only replace in basename
            QString baseName = fileName.left(dotIndex);
            QString extension = fileName.mid(dotIndex);
            baseName.replace(regex, replacementWithTags);
            return baseName + extension;
        } else {
            // No extension - replace entire filename
            QString result = fileName;
            result.replace(regex, replacementWithTags);
            return result;
        }
    }
    return fileName;
}

QString PrefixOperation::perform(const QString &fileName, int fileIndex) const
{
    QString prefixWithTags = replaceTags(m_prefix, fileIndex);
    return prefixWithTags + fileName;
}

QString SuffixOperation::perform(const QString &fileName, int fileIndex) const
{
    QString suffixWithTags = replaceTags(m_suffix, fileIndex);
    
    // Add suffix before extension
    // Note: dotIndex > 0 ensures we don't treat dotfiles (like .bashrc) as having extensions
    int dotIndex = fileName.lastIndexOf('.');
    if (dotIndex > 0) {
        return fileName.left(dotIndex) + suffixWithTags + fileName.mid(dotIndex);
    } else {
        return fileName + suffixWithTags;
    }
}

QString InsertOperation::perform(const QString &fileName, int fileIndex) const
{
    QString textWithTags = replaceTags(m_text, fileIndex);
    
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
    
    baseName.insert(pos, textWithTags);
    return baseName + extension;
}

QString ChangeExtensionOperation::perform(const QString &fileName, int fileIndex) const
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
