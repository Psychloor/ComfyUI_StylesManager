//
// Created by blomq on 2025-07-07.
//

#ifndef NEWENTRYDIALOG_H
#define NEWENTRYDIALOG_H

#include <QDialog>

class prompt_name_validator;

QT_BEGIN_NAMESPACE
namespace Ui {
    class new_entry_dialog; }
QT_END_NAMESPACE

class new_entry_dialog final : public QDialog {
Q_OBJECT

public:
    explicit new_entry_dialog(QWidget *parent = nullptr);
    ~new_entry_dialog() override;

    void set_name(const QString& name) const;
    void set_confirm_button_text(const QString& text) const;

    [[nodiscard]] QString get_name() const;

    public slots:
    void confirm_clicked();
    void cancel_clicked();

private:
    void validate_input() const;

    std::unique_ptr<Ui::new_entry_dialog> ui;
    std::unique_ptr<prompt_name_validator> _validator;
};


#endif //NEWENTRYDIALOG_H
