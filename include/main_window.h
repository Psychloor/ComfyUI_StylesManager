//
// Created by blomq on 2025-07-07.
//

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QDataWidgetMapper>
#include <qfileinfo.h>
#include <QSortFilterProxyModel>

#include "prompt_entry.h"
#include "third_party/csv/csv.h"
#include "include/prompt_entry_list_model.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class main_window;
}
QT_END_NAMESPACE

class main_window final : public QMainWindow
{
    Q_OBJECT

public:
    explicit main_window(QWidget* parent = nullptr);
    ~main_window() override;

protected:
    void closeEvent(QCloseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private slots:
    void prompt_index_changed(int index) const;
    void add_entry_clicked();
    void rename_entry_clicked();
    void remove_entry_clicked();

    void new_file_clicked();
    void import_csv_clicked();
    void open_clicked();
    void save_clicked();
    void save_as_clicked();
    void close_clicked();

    void remove_duplicates_clicked();

    void model_data_changed(const QModelIndex& top_left, const QModelIndex& bottom_right,
                            const QList<int>& roles = QList<int>());

private:
    void save_json_file(const QString& file_path);
    void setup_mapper();
    void update_recent_files_menu();
     [[nodiscard]] bool check_unsaved_changes();
    bool load_file(const QString& file_path);
    bool load_csv_file(const QString& file_path);

    static constexpr auto window_title = " - Prompt Styles Manager";
    static constexpr auto untitled = "Untitled[*]";
    static constexpr auto untitled_window_title = "Untitled[*] - Prompt Styles Manager";


private:
    std::unique_ptr<Ui::main_window> _ui;
    std::unique_ptr<QMenu> _recent_files_menu;

    QFileInfo _current_file_info;
    std::unique_ptr<ns::prompt_entry_list_model> _model;
    std::unique_ptr<QSortFilterProxyModel> _proxy_model;
    std::unique_ptr<QDataWidgetMapper> _mapper;
};


#endif //MAIN_WINDOW_H
