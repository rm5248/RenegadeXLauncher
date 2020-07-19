#ifndef INSTRUCTIONENTRY_H
#define INSTRUCTIONENTRY_H

#include <QJsonObject>
#include <QDateTime>

class InstructionEntry
{
public:
    InstructionEntry();
    InstructionEntry( QJsonObject obj );

    QString path() const;
    QString oldHash() const;
    QString newHash() const;
    QString compressedHash() const;
    QString deltaHash() const;
    QDateTime oldLastWriteTime() const;
    QDateTime newLastWriteTime() const;
    int fullReplaceSize() const;
    int deltaSize() const;
    bool hasDelta() const;

private:
    QString m_path;
    QString m_oldHash;
    QString m_newHash;
    QString m_compressedHash;
    QString m_deltaHash;
    QDateTime m_oldLastWriteTime;
    QDateTime m_newLastWriteTime;
    int m_fullReplaceSize;
    int m_deltaSize;
    bool m_hasDelta;
};

#endif // INSTRUCTIONENTRY_H
