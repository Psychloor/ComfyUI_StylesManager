#pragma once

#include <QAbstractListModel>
#include "prompt_entry.h"

namespace ns
{
	enum prompt_entry_roles
	{
		pr_name_role = Qt::DisplayRole,
		pr_prompt_role = Qt::UserRole + 1,
		pr_negative_prompt_role,
	};

	class prompt_entry_list_model final : public QAbstractListModel
	{
		Q_OBJECT

	public:
		explicit prompt_entry_list_model(QObject* parent = nullptr)
			: QAbstractListModel(parent)
		{
		}

		[[nodiscard]] int rowCount(const QModelIndex& parent) const override
		{
			return parent.isValid() ? 0 : static_cast<int>(_entries.size());
		}

		[[nodiscard]] QHash<int, QByteArray> roleNames() const override
		{
			QHash<int, QByteArray> roles;
			roles[pr_name_role] = "name";
			roles[pr_prompt_role] = "prompt";
			roles[pr_negative_prompt_role] = "negativePrompt";
			return roles;
		}


		[[nodiscard]] QVariant data(const QModelIndex& index, const int role) const override
		{
			if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(_entries.size()))
				return {};

			const auto& [name, prompt, negative_prompt] = _entries[index.row()];

			switch (role)
			{
			case pr_name_role:
			case Qt::EditRole:
				return name;
			case pr_prompt_role:
				return prompt;
			case pr_negative_prompt_role:
				return negative_prompt;
			default:
				return {};
			}
		}


		bool setData(const QModelIndex& index, const QVariant& value, const int role) override
		{
			if (!index.isValid()) return false;
			auto& [name, prompt, negative_prompt] = _entries[index.row()];

			switch (role)
			{
			case pr_name_role:
				name = value.toString();
				break;
			case pr_prompt_role:
				prompt = value.toString();
				break;
			case pr_negative_prompt_role:
				negative_prompt = value.toString();
				break;
			default:
				return false;
			}

			emit dataChanged(index, index, {role});
			return true;
		}

		[[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const override
		{
			if (!index.isValid())
				return Qt::NoItemFlags;
			return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
		}

		[[nodiscard]] const QVector<prompt_entry>& entries() const { return _entries; }
		[[nodiscard]] QVector<prompt_entry>& entries() { return _entries; }

		void set_entries(QVector<prompt_entry>& entries)
		{
			beginResetModel();
			_entries.clear();
			_entries = std::move(entries);
			endResetModel();
		}

		void add_entry(const prompt_entry& entry)
		{
			beginInsertRows(QModelIndex(), static_cast<int>(_entries.size()), static_cast<int>(_entries.size()));
			_entries.push_back(entry);
			endInsertRows();
		}

		void remove_entry(const int index)
		{
			if (index >= 0 && index < static_cast<int>(_entries.size()))
			{
				beginRemoveRows(QModelIndex(), index, index);
				_entries.erase(_entries.begin() + index);
				endRemoveRows();
			}
		}

		void clear()
		{
			beginResetModel();
			_entries.clear();
			endResetModel();
		}

		[[nodiscard]] const prompt_entry& at(const qsizetype index) const { return _entries.at(index); }

	private:
		QVector<prompt_entry> _entries;
	};
} // namespace ns
