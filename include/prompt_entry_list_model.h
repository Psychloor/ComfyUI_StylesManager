#pragma once

#include <QAbstractListModel>
#include "prompt_entry.h"

enum class prompt_entry_roles
{
	name = Qt::DisplayRole,
	prompt = Qt::UserRole + 1,
	negative_prompt = Qt::UserRole + 2,
};

class prompt_entry_list_model final : public QAbstractListModel
{
	Q_OBJECT // NOLINT(*-identifier-length)

public:
	explicit prompt_entry_list_model(QObject* parent = nullptr);

	[[nodiscard]] int rowCount(const QModelIndex& parent) const override;
	[[nodiscard]] QHash<int, QByteArray> roleNames() const override;

	[[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;;
	bool setData(const QModelIndex& index, const QVariant& value, int role) override;

	[[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const override;

	[[nodiscard]] const QVector<prompt_entry>& entries() const { return _entries; }
	[[nodiscard]] QVector<prompt_entry>& entries() { return _entries; }

	void set_entries(QVector<prompt_entry>& entries);

	void add_entry(const prompt_entry& entry);
	void remove_entry(int index);

	void clear();

	[[nodiscard]] const prompt_entry& at(const qsizetype index) const { return _entries.at(index); }

private:
	QVector<prompt_entry> _entries;
};

// namespace ns
