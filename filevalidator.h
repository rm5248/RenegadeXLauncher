#ifndef FILEVALIDATOR_H
#define FILEVALIDATOR_H

#include <QObject>
#include <QFile>

class FileValidator : public QObject
{
    Q_OBJECT
public:
    explicit FileValidator( QString fileToChecksum,
                            QString expectedChecksum,
                            QObject *parent = nullptr);

    QString calculatedChecksum() const;
    QString expectedChecksum() const;

signals:
    void checksumCompleted( bool success, QString calculated );

public slots:
    void startChecksum();

private:
    QFile m_file;
    QString m_expected;
    QString m_checksum;
};

#endif // FILEVALIDATOR_H
