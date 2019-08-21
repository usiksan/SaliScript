#include "DTextEditorSettings.h"
#include "WCModeEditor.h"
#include "ui_DTextEditorSettings.h"

DTextEditorSettings::DTextEditorSettings(WCModeEditor *pEditor, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DTextEditorSettings)

{
    ui->setupUi(this);
    ui->autoCompleteCheckBox->setChecked(pEditor->autoCompleteParenthesis());
    ui->autoIndentSpinBox->setValue(pEditor->autoIndentSpaceCount());
}


DTextEditorSettings::~DTextEditorSettings()
{
    delete ui;
}

int DTextEditorSettings::autoIndentSpaceCount() const
{
    return ui->autoIndentSpinBox->value();
}

bool DTextEditorSettings::autoIndentParenthesis() const
{
    return ui->autoCompleteCheckBox->checkState() == Qt::Checked;
}
