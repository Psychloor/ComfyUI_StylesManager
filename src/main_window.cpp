//
// Created by blomq on 2025-07-07.
//

// You may need to build the project (run Qt uic code generator) to get "ui_prompt_window.h" resolved

#include "include/main_window.h"

#include "ui_main_window.h"
#include <QFileDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QMimeData>

#include "settings.h"
#include "include/new_entry_dialog.h"


main_window::main_window(QWidget* parent) :
	QMainWindow(parent)
{
	_ui = std::make_unique<Ui::main_window>();
	_ui->setupUi(this);
	setAcceptDrops(true);

	_model = std::make_unique<ns::prompt_entry_list_model>();

	_proxy_model = std::make_unique<QSortFilterProxyModel>(this);
	_proxy_model->setSourceModel(_model.get());
	_proxy_model->setFilterCaseSensitivity(Qt::CaseInsensitive);

	_ui->promptnameComboBox->setModel(_proxy_model.get());

	_ui->promptTextEdit->setEnabled(false);
	_ui->negativePromptTextEdit->setEnabled(false);

	// Connect combo box
	connect(_ui->promptnameComboBox, &QComboBox::currentIndexChanged, this, &main_window::prompt_index_changed);

	connect(_ui->filterLineEdit, &QLineEdit::textChanged,
			_proxy_model.get(), &QSortFilterProxyModel::setFilterFixedString);

	// Connect buttons
	connect(_ui->addEntryButton, &QPushButton::clicked,
			this, &main_window::add_entry_clicked);

	connect(_ui->renameEntryButton, &QPushButton::clicked, this, &main_window::rename_entry_clicked);

	connect(_ui->removeEntryButton, &QPushButton::clicked,
			this, &main_window::remove_entry_clicked);

	// Connect actions
	connect(_ui->actionNew, &QAction::triggered, this, &main_window::new_file_clicked);
	connect(_ui->actionImportCSV, &QAction::triggered, this, &main_window::import_csv_clicked);
	connect(_ui->actionOpen, &QAction::triggered, this, &main_window::open_clicked);
	connect(_ui->actionSave, &QAction::triggered, this, &main_window::save_clicked);
	connect(_ui->actionSaveAs, &QAction::triggered, this, &main_window::save_as_clicked);
	connect(_ui->actionClose, &QAction::triggered, this, &main_window::close_clicked);
	connect(_ui->actionRemoveDuplicates, &QAction::triggered, this, &main_window::remove_duplicates_clicked);

	connect(_model.get(), &ns::prompt_entry_list_model::dataChanged, this, &main_window::model_data_changed);

	// Setup recent files menu
	_recent_files_menu = std::make_unique<QMenu>("Recent Files", this);
	_ui->menuFile->insertMenu(_ui->actionOpen, _recent_files_menu.get());
	update_recent_files_menu();
}

main_window::~main_window() = default;

void main_window::closeEvent(QCloseEvent* event)
{
	if (check_unsaved_changes())
	{
		event->accept();
	}
	else
	{
		event->ignore();
	}
}

void main_window::dragEnterEvent(QDragEnterEvent* event)
{
	if (event->mimeData()->hasUrls())
	{
		if (const QList<QUrl> urls = event->mimeData()->urls(); urls.size() == 1)
		{
			const QString file_path = urls.first().toLocalFile();

			const QString extension = QFileInfo(file_path).suffix().toLower();
			if (extension == "json" || extension == "csv")
			{
				event->acceptProposedAction();
				return;
			}
		}
	}
	event->ignore();
}

void main_window::dropEvent(QDropEvent* event)
{
	const QString file_path = event->mimeData()->urls().first().toLocalFile();
	const QString extension = QFileInfo(file_path).suffix().toLower();

	if (!check_unsaved_changes())
	{
		event->ignore();
		return;
	}

	bool success = false;
	if (extension == "json")
	{
		success = load_json_file(file_path);
	}
	else if (extension == "csv")
	{
		success = load_csv_file(file_path);
	}

	if (success)
	{
		event->acceptProposedAction();
	}
	else
	{
		event->ignore();
	}
}

void main_window::save_json_file(const QString& file_path)
{
	QFile file(file_path);
	if (!file.open(QIODevice::WriteOnly))
	{
		QMessageBox::warning(this, tr("Error"),
							 tr("Could not open file for writing: %1").arg(file.errorString()));
		return;
	}

	try
	{
		nlohmann::ordered_json j = nlohmann::ordered_json::array();
		Q_FOREACH(const auto& entry, _model->entries())
		{
			j.push_back(entry);
		}

		// Write JSON using QFile
		if (const QByteArray json_data = QByteArray::fromStdString(j.dump(4)); file.write(json_data) == -1)
		{
			throw std::runtime_error("Failed to write to file");
		}

		file.close();

		_current_file_info.setFile(file_path);
		setWindowTitle(QFileInfo(file).fileName() + "[*]" + window_title);
		setWindowModified(false);

		settings::instance().add_recent_file(file_path);
		update_recent_files_menu();

		_ui->statusbar->showMessage(tr("File saved successfully"), 3000);
	}
	catch (const std::exception& e)
	{
		QMessageBox::critical(this, tr("Save Error"),
							  tr("Failed to save file: %1").arg(e.what()));
	}
}

void main_window::setup_mapper()
{
	if (!_mapper)
	{
		_mapper = std::make_unique<QDataWidgetMapper>(this);
		_mapper->setModel(_model.get());
		_mapper->addMapping(_ui->promptnameComboBox, ns::pr_name_role);

		// For QPlainTextEdit, we need to handle it differently
		connect(_mapper.get(), &QDataWidgetMapper::currentIndexChanged,
				this, [this](const int idx)
				{
					if (idx >= 0)
					{
						const QModelIndex modelIdx = _model->index(idx);
						_ui->promptTextEdit->setPlainText(
							_model->data(modelIdx, ns::pr_prompt_role).toString());
						_ui->negativePromptTextEdit->setPlainText(
							_model->data(modelIdx, ns::pr_negative_prompt_role).toString());
					}
				});

		// Connect text edits back to model
		connect(_ui->promptTextEdit, &QPlainTextEdit::textChanged,
				this, [this]()
				{
					if (_mapper->currentIndex() >= 0)
					{
						const QModelIndex idx = _model->index(_mapper->currentIndex());
						_model->setData(idx, _ui->promptTextEdit->toPlainText(), ns::pr_prompt_role);
					}
				});

		connect(_ui->negativePromptTextEdit, &QPlainTextEdit::textChanged,
				this, [this]()
				{
					if (_mapper->currentIndex() >= 0)
					{
						const QModelIndex idx = _model->index(_mapper->currentIndex());
						_model->setData(idx, _ui->negativePromptTextEdit->toPlainText(), ns::pr_negative_prompt_role);
					}
				});

		// Connect to submit changes immediately when editing
		_mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
	}

	if (_model->rowCount(QModelIndex()
	) > 0)
	{
		_mapper->toFirst();
	}
}

void main_window::update_recent_files_menu()
{
	_recent_files_menu->clear();

	const auto recent_files = settings::instance().recent_files();
	for (const QString& file : recent_files)
	{
		QAction* action = _recent_files_menu->addAction(QFileInfo(file).fileName());
		action->setData(file);
		action->setStatusTip(file);

		connect(action, &QAction::triggered, this, [this, file]()
		{
			if (check_unsaved_changes())
			{
				load_json_file(file);
			}
		});
	}

	_recent_files_menu->setEnabled(!recent_files.isEmpty());
}

bool main_window::check_unsaved_changes()
{
	if (!isWindowModified())
	{
		return true;
	}

	const auto reply = QMessageBox::question(this,
											 tr("Unsaved Changes"),
											 tr("You have unsaved changes. Do you want to save them first?"),
											 QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

	if (reply == QMessageBox::Cancel)
	{
		return false;
	}

	if (reply == QMessageBox::Save)
	{
		save_clicked();

		// If the file is still modified after trying to save,
		// it means the save was canceled or failed
		if (isWindowModified())
		{
			return false;
		}
	}

	return true;
}

bool main_window::load_json_file(const QString& file_path)
{
	QFile file(file_path);
	if (!file.exists())
	{
		QMessageBox::warning(this,
							 tr("File Not Found"),
							 tr("The file \"%1\" does not exist.").arg(file_path));
		return false;
	}

	if (!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::warning(this,
							 tr("Error"),
							 tr("Could not open file for reading: %1").arg(file.errorString()));
		return false;
	}

	_model->clear();
	_ui->promptTextEdit->clear();
	_ui->negativePromptTextEdit->clear();

	try
	{
		const QByteArray data = file.readAll();
		nlohmann::json j = nlohmann::json::parse(data.toStdString());

		for (const auto& entry : j)
		{
			const auto name = entry["name"].get<QString>();
			const auto prompt = entry["prompt"].get<QString>();
			const auto negative = entry["negative_prompt"].get<QString>();

			_model->add_entry(ns::prompt_entry(name, prompt, negative));
		}

		if (_model->rowCount(QModelIndex()) > 0)
		{
			setup_mapper();
			_ui->promptnameComboBox->setCurrentIndex(0);
		}

		_current_file_info.setFile(file_path);
		settings::instance().add_recent_file(file_path);
		settings::instance().set_last_directory(QFileInfo(file_path).absolutePath());

		setWindowTitle(std::format("{}[*] - Prompt Styles Manager",
								   QFileInfo(file).fileName().toStdString()).data());
		setWindowModified(false);

		_ui->statusbar->showMessage(tr("File loaded successfully"), 3000);
		return true;
	}
	catch (const std::exception& ex)
	{
		QMessageBox::warning(this,
							 tr("Error"),
							 tr("Failed to load file: %1").arg(ex.what()));
		return false;
	}
}


bool main_window::load_csv_file(const QString& file_path)
{
	if (!QFile::exists(file_path))
	{
		QMessageBox::warning(this, tr("File not found"), tr("File not found"));
		return false;
	}

	try
	{
		_model->clear();

		for (csv::CSVReader reader(file_path.toStdString()); csv::CSVRow& row : reader)
		{
			auto name = row["name"].get<std::string>();
			auto prompt = row["prompt"].get<std::string>();
			auto negative = row["negative_prompt"].get<std::string>();

			if (!prompt.empty() && prompt.find("{prompt}") == std::string::npos)
			{
				std::string temp = "{prompt} ";
				temp += prompt;
				prompt = std::move(temp);
			}

			_model->add_entry(ns::prompt_entry(
				QString::fromStdString(name),
				QString::fromStdString(prompt),
				QString::fromStdString(negative)));
		}

		setup_mapper();

		_current_file_info.setFile(""); // Reset file info since this is an import
		setWindowTitle(untitled_window_title);
		setWindowModified(true);

		settings::instance().set_last_directory(QFileInfo(file_path).absolutePath());
		_ui->statusbar->showMessage(tr("CSV file imported successfully"), 3000);
		return true;
	}
	catch (const std::exception& ex)
	{
		QMessageBox::warning(this, tr("CSV parsing error"),
							 tr("CSV parsing error: %1").arg(ex.what()));
		return false;
	}
}

void main_window::prompt_index_changed(const int index) const
{
	const auto enabled = index >= 0 && index < _model->rowCount(QModelIndex());
	_ui->promptTextEdit->setEnabled(enabled);
	_ui->negativePromptTextEdit->setEnabled(enabled);

	if (!enabled)
	{
		_ui->promptTextEdit->clear();
		_ui->negativePromptTextEdit->clear();
		return;
	}

	// Convert proxy index to source index
	const auto source_index = _proxy_model->mapToSource(_proxy_model->index(index, 0)).row();
	_mapper->setCurrentIndex(source_index);
}

void main_window::add_entry_clicked()
{
	new_entry_dialog dialog(this);
	dialog.setWindowTitle("Add Entry");
	dialog.set_confirm_button_text("Add");

	if (dialog.exec() != QDialog::Accepted) return;

	_model->add_entry({dialog.get_name(), "{prompt}", ""});

	if (_model->rowCount(QModelIndex()) == 1)
	{
		// If this is the first entry
		setup_mapper();
	}

	_ui->promptnameComboBox->setCurrentIndex(_model->rowCount(QModelIndex()) - 1);
}

void main_window::rename_entry_clicked()
{
	// Convert proxy index to source index
	const auto selected_index = _ui->promptnameComboBox->currentIndex();
	const auto source_index = _proxy_model->mapToSource(_proxy_model->index(selected_index, 0)).row();

	if (source_index < 0 || source_index >= _model->rowCount(QModelIndex())) return;

	const auto model_index = _model->index(source_index);
	const auto old_name = _model->data(model_index, ns::prompt_entry_roles::pr_name_role).toString();

	new_entry_dialog dialog(this);
	dialog.setWindowTitle("Rename Entry");
	dialog.set_name(old_name);
	dialog.set_confirm_button_text("Rename");

	if (dialog.exec() != QDialog::Accepted) return;

	const auto name = dialog.get_name().toStdString();
	_model->setData(model_index, QString::fromStdString(name), ns::prompt_entry_roles::pr_name_role);
}

void main_window::remove_entry_clicked()
{
	const auto selected_index = _ui->promptnameComboBox->currentIndex();
	if (selected_index < 0 || selected_index >= _model->rowCount(QModelIndex())) return;

	const auto reply = QMessageBox::question(this, _model->at(selected_index).name,
											 "Are you sure you want to remove this entry?",
											 QMessageBox::Yes | QMessageBox::No);
	if (reply == QMessageBox::No) return;
	_model->remove_entry(selected_index);
}

void main_window::new_file_clicked()
{
	if (!check_unsaved_changes())
	{
		return;
	}

	_model->clear();
	_current_file_info.setFile("");
	setWindowTitle(untitled_window_title);
	setWindowModified(false);
}

void main_window::import_csv_clicked()
{
	if (!check_unsaved_changes())
	{
		return;
	}

	const QString file_name = QFileDialog::getOpenFileName(this,
														   tr("Import CSV"),
														   settings::instance().last_directory(),
														   tr("CSV Files (*.csv);;All Files (*)"));

	if (!file_name.isEmpty())
	{
		load_csv_file(file_name);
	}
}

void main_window::open_clicked()
{
	if (!check_unsaved_changes())
	{
		return;
	}

	const QString file_name = QFileDialog::getOpenFileName(this,
														   tr("Open Prompts"),
														   settings::instance().last_directory(),
														   tr("JSON Files (*.json);;All Files (*)"));

	if (!file_name.isEmpty())
	{
		load_json_file(file_name);
	}
}

void main_window::save_clicked()
{
	if (_model->rowCount(QModelIndex()) == 0)
	{
		QMessageBox::warning(this, "No entries", "No entries to save");
		return;
	}

	if (_current_file_info.filePath().isEmpty())
	{
		save_as_clicked();
		return;
	}

	save_json_file(_current_file_info.filePath());
}

void main_window::save_as_clicked()
{
	if (_model->rowCount(QModelIndex()) == 0)
	{
		QMessageBox::warning(this, "No entries", "No entries to save");
		return;
	}

	const QString file_save_name = QFileDialog::getSaveFileName(this,
																tr("Save Prompts"),
																settings::instance().last_directory(),
																tr("JSON Files (*.json);;All Files (*)"));

	if (file_save_name.isEmpty())
	{
		return;
	}

	// Update the last directory
	settings::instance().set_last_directory(QFileInfo(file_save_name).absolutePath());
	settings::instance().add_recent_file(file_save_name);

	_current_file_info.setFile(file_save_name);
	save_json_file(_current_file_info.filePath());

	const auto file_name = QFileInfo(file_save_name).fileName();
	setWindowTitle(file_name + "[*] - Prompt Styles Manager");
	setWindowModified(false);
}

void main_window::close_clicked()
{
	close();
}

void main_window::remove_duplicates_clicked()
{
	std::unordered_set<QString> unique_names;
	std::vector<ns::prompt_entry> unique_entries;

	Q_FOREACH(const auto& entry, _model->entries())
	{
		if (auto [it, inserted] = unique_names.insert(entry.name); inserted)
		{
			unique_entries.push_back(entry);
		}
	}

	_model->set_entries(unique_entries);
	_ui->promptnameComboBox->clear();
	_ui->promptTextEdit->clear();
	_ui->negativePromptTextEdit->clear();

	setup_mapper();
	setWindowModified(true);
}

void main_window::model_data_changed(const QModelIndex&, const QModelIndex&,
									 const QList<int>&)
{
	setWindowModified(true);
}
