#ifndef OPERATION_H
#define OPERATION_H

#include <QString>
#include <memory>

/**
 * @brief Abstract base class for file name operations.
 * 
 * Each operation can be applied to a filename to produce a transformed filename.
 * Concrete operation classes must implement the perform() method.
 */
class Operation
{
public:
    virtual ~Operation() = default;
    
    /**
     * @brief Apply this operation to the given filename.
     * @param fileName The input filename to transform
     * @return The transformed filename
     */
    virtual QString perform(const QString &fileName) const = 0;
    
    /**
     * @brief Get the operation type identifier.
     * @return A string identifying the operation type (e.g., "replace", "prefix")
     */
    virtual QString getType() const = 0;
};

/**
 * @brief Replace operation using regular expressions.
 * 
 * Replaces all matches of a regex pattern with a replacement string.
 */
class ReplaceOperation : public Operation
{
public:
    ReplaceOperation(const QString &pattern, const QString &replacement)
        : m_pattern(pattern), m_replacement(replacement) {}
    
    QString perform(const QString &fileName) const override;
    QString getType() const override { return "replace"; }
    
    QString getPattern() const { return m_pattern; }
    QString getReplacement() const { return m_replacement; }
    
private:
    QString m_pattern;
    QString m_replacement;
};

/**
 * @brief Prefix operation - adds text to the beginning of the filename.
 */
class PrefixOperation : public Operation
{
public:
    explicit PrefixOperation(const QString &prefix)
        : m_prefix(prefix) {}
    
    QString perform(const QString &fileName) const override;
    QString getType() const override { return "prefix"; }
    
    QString getPrefix() const { return m_prefix; }
    
private:
    QString m_prefix;
};

/**
 * @brief Suffix operation - adds text before the file extension.
 */
class SuffixOperation : public Operation
{
public:
    explicit SuffixOperation(const QString &suffix)
        : m_suffix(suffix) {}
    
    QString perform(const QString &fileName) const override;
    QString getType() const override { return "suffix"; }
    
    QString getSuffix() const { return m_suffix; }
    
private:
    QString m_suffix;
};

/**
 * @brief Insert operation - inserts text at a specified position.
 */
class InsertOperation : public Operation
{
public:
    InsertOperation(int position, const QString &text)
        : m_position(position), m_text(text) {}
    
    QString perform(const QString &fileName) const override;
    QString getType() const override { return "insert"; }
    
    int getPosition() const { return m_position; }
    QString getText() const { return m_text; }
    
private:
    int m_position;
    QString m_text;
};

/**
 * @brief Change extension operation - changes the file extension.
 */
class ChangeExtensionOperation : public Operation
{
public:
    explicit ChangeExtensionOperation(const QString &newExtension)
        : m_newExtension(newExtension) {}
    
    QString perform(const QString &fileName) const override;
    QString getType() const override { return "change_ext"; }
    
    QString getNewExtension() const { return m_newExtension; }
    
private:
    QString m_newExtension;
};

#endif // OPERATION_H
