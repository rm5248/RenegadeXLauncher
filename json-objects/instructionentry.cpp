#include "instructionentry.h"

InstructionEntry::InstructionEntry()
{

}

InstructionEntry::InstructionEntry( QJsonObject obj ){
    m_path = obj[ "Path" ].toString();
    m_oldHash = obj[ "OldHash" ].toString();
    m_newHash = obj[ "NewHash" ].toString();
    m_compressedHash = obj[ "CompressedHash" ].toString();
    m_deltaHash = obj[ "DeltaHash" ].toString();
    m_oldLastWriteTime = QDateTime::fromString( obj[ "OldLastWriteTime" ].toString(), Qt::ISODate );
    m_newLastWriteTime = QDateTime::fromString( obj[ "OldLastWriteTime" ].toString(), Qt::ISODate );
    m_fullReplaceSize = obj[ "FullReplaceSize" ].toInt();
    m_deltaSize = obj[ "DeltaSize" ].toInt();
    m_hasDelta = obj[ "HasDelta" ].toBool();
}

QString InstructionEntry::path() const {
    return m_path;
}

QString InstructionEntry::oldHash() const {
    return m_oldHash;
}

QString InstructionEntry::newHash() const {
    return m_newHash;
}

QString InstructionEntry::compressedHash() const {
    return m_compressedHash;
}

QString InstructionEntry::deltaHash() const {
    return m_deltaHash;
}

QDateTime InstructionEntry::oldLastWriteTime() const {
    return m_oldLastWriteTime;
}

QDateTime InstructionEntry::newLastWriteTime() const {
    return m_newLastWriteTime;
}

int InstructionEntry::fullReplaceSize() const {
    return m_fullReplaceSize;
}

int InstructionEntry::deltaSize() const {
    return m_deltaSize;
}

bool InstructionEntry::hasDelta() const {
    return m_hasDelta;
}
