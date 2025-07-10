//
// Created by blomq on 2025-07-07.
//

// You may need to build the project (run Qt uic code generator) to get "ui_NewEntryDialog.h" resolved

#include "include/new_entry_dialog.h"

#include <QMessageBox>

#include "include/prompt_name_validator.h"
#include "ui_new_entry_dialog.h"


new_entry_dialog::new_entry_dialog(QWidget* parent) :
	QDialog(parent)
{
	_ui = std::make_unique<Ui::new_entry_dialog>();
	_ui->setupUi(this);

	_validator = std::make_unique<prompt_name_validator>();
	_ui->promptNameLineEdit->setValidator(_validator.get());
	_ui->confirmButton->setEnabled(false);

	connect(_ui->confirmButton, &QPushButton::clicked, this, &new_entry_dialog::confirm_clicked);
	connect(_ui->cancelButton, &QPushButton::clicked, this, &new_entry_dialog::cancel_clicked);

	connect(_ui->promptNameLineEdit, &QLineEdit::textChanged, this, &new_entry_dialog::validate_input);
}

new_entry_dialog::~new_entry_dialog() = default;

void new_entry_dialog::set_name(const QString& name) const
{
	_ui->promptNameLineEdit->setText(name);
}

void new_entry_dialog::set_confirm_button_text(const QString& text) const
{
	_ui->confirmButton->setText(text);
}

QString new_entry_dialog::get_name() const
{
	return _ui->promptNameLineEdit->text();
}

void new_entry_dialog::confirm_clicked()
{
	accept();
}

void new_entry_dialog::cancel_clicked()
{
	reject();
}

void new_entry_dialog::validate_input() const
{
	int pos = 0;
	QString text = _ui->promptNameLineEdit->text();
	const auto state = _validator->validate(text, pos);
	_ui->confirmButton->setEnabled(state == QValidator::Acceptable);
}
