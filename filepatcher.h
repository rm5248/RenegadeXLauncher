#ifndef FILEPATCHER_H
#define FILEPATCHER_H

#include <QObject>
#include <QProcess>
#include <QFile>

class FilePatcher : public QObject
{
    Q_OBJECT
public:
    explicit FilePatcher(QObject *parent = nullptr);

    void setInputFile(QString absolutePath);
    void setOutputFile(QString absolutePath);
    void doPatch();

signals:
    void filePatched();

private slots:
    void finished(int exitCode, QProcess::ExitStatus status);

private:
    QString m_inputFile;
    QString m_outputFile;
    QProcess m_xdeltaProcess;
};

#endif // FILEPATCHER_H
