#ifndef DTEXTEDITORSETTINGS_H
#define DTEXTEDITORSETTINGS_H

#include <QDialog>

namespace Ui {
class DTextEditorSettings;
}

class WCModeEditor;
class DTextEditorSettings : public QDialog
{
    Q_OBJECT

public:
    explicit DTextEditorSettings(WCModeEditor* pEditor, QWidget *parent = 0);
    ~DTextEditorSettings();

    int autoIndentSpaceCount()const;
    bool autoIndentParenthesis()const;

private:
    Ui::DTextEditorSettings *ui;
};

#endif // DTEXTEDITORSETTINGS_H
