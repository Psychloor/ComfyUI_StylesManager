//
// Created by blomq on 2025-07-10.
//

#include "include/prompt_entry_list_model.h"

prompt_entry_list_model::prompt_entry_list_model(QObject* parent): QAbstractListModel(parent)
{
}

int prompt_entry_list_model::rowCount(const QModelIndex& parent) const
{
	return parent.isValid() ? 0 : static_cast<int>(_entries.size());
}

QHash<int, QByteArray> prompt_entry_list_model::roleNames() const
{
	QHash<int, QByteArray> roles;
	roles[static_cast<int>(prompt_entry_roles::name)] = "name";
	roles[static_cast<int>(prompt_entry_roles::prompt)] = "prompt";
	roles[static_cast<int>(prompt_entry_roles::negative_prompt)] = "negativePrompt";
	return roles;
}

QVariant prompt_entry_list_model::data(const QModelIndex& index, const int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(_entries.size()))
	{
		return {};
	}

	const auto& [name, prompt, negative_prompt] = _entries[index.row()];

	switch (role)
	{
	case static_cast<int>(prompt_entry_roles::name):
	case Qt::EditRole:
		return name;
	case static_cast<int>(prompt_entry_roles::prompt):
		return prompt;
	case static_cast<int>(prompt_entry_roles::negative_prompt):
		return negative_prompt;
	default:
		return {};
	}
}

bool prompt_entry_list_model::setData(const QModelIndex& index, const QVariant& value, const int role)
{
	if (!index.isValid())
	{
		return false;
	}
	auto& [name, prompt, negative_prompt] = _entries[index.row()];

	switch (role)
	{
	case static_cast<int>(prompt_entry_roles::name):
		name = value.toString();
		break;
	case static_cast<int>(prompt_entry_roles::prompt):
		prompt = value.toString();
		break;
	case static_cast<int>(prompt_entry_roles::negative_prompt):
		negative_prompt = value.toString();
		break;
	default:
		return false;
	}

	emit dataChanged(index, index, {role});
	return true;
}

Qt::ItemFlags prompt_entry_list_model::flags(const QModelIndex& index) const
{
	if (!index.isValid())
	{
		return Qt::NoItemFlags;
	}
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

void prompt_entry_list_model::set_entries(QVector<prompt_entry>& entries)
{
	beginResetModel();
	_entries.clear();
	_entries = std::move(entries);
	endResetModel();
}

void prompt_entry_list_model::add_entry(const prompt_entry& entry)
{
	beginInsertRows(QModelIndex(), static_cast<int>(_entries.size()), static_cast<int>(_entries.size()));
	_entries.push_back(entry);
	endInsertRows();
}

void prompt_entry_list_model::remove_entry(const int index)
{
	if (index >= 0 && index < static_cast<int>(_entries.size()))
	{
		beginRemoveRows(QModelIndex(), index, index);
		_entries.erase(_entries.begin() + index);
		endRemoveRows();
	}
}

void prompt_entry_list_model::clear()
{
	beginResetModel();
	_entries.clear();
	endResetModel();
}
