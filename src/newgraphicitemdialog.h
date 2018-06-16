#ifndef NEWGRAPHICITEMDIALOG_H
#define NEWGRAPHICITEMDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QLabel>

#include <QtGlobal>
#if QT_VERSION >= 0x050000
    #include <QJsonObject>
#else
    #include <QMap>
#endif

namespace Ui {
class NewGraphicItemDialog;
}

class NewGraphicItemDialog : public QDialog
{
    Q_OBJECT

public:
#if QT_VERSION >= 0x050000
    explicit NewGraphicItemDialog(QWidget *parent = 0, QJsonObject *results = 0);
#else
    explicit NewGraphicItemDialog(QWidget *parent = 0, QMap <QString, QVariant> *results = 0);
#endif

    ~NewGraphicItemDialog();

private slots:
    void on_PastetoolButton_clicked();

    void on_toolButton_clicked();

    void on_buttonBox_accepted();

private:
    Ui::NewGraphicItemDialog *ui;

#if QT_VERSION >= 0x050000
    QJsonObject *completed;
#else
    QMap <QString, QVariant> *completed;
#endif

    QString m_imagefilename;
};

class PreviewFileDialog : public QFileDialog
{
        Q_OBJECT
    public:
        explicit PreviewFileDialog(
            QWidget* parent = 0,
            const QString & caption = QString(),
            const QString & directory = QString(),
            const QString & filter = QString()
        );

    protected slots:
        void OnCurrentChanged(const QString & path);

    protected:
        QLabel* mpPreview;

};

#endif // NEWGRAPHICITEMDIALOG_H
