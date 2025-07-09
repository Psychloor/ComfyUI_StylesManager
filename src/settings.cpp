//
// Created by blomq on 2025-07-08.
//

#include "include/settings.h"

#include <QSettings>

settings& settings::instance()
{
	static settings instance;
	if (!instance._loaded)
	{
		instance.load();
		instance._loaded = true;
	}

	return instance;
}

QStringList settings::recent_files() const
{
	return _recent_files;
}

void settings::add_recent_file(const QString& path)
{
	// Remove if already exists
	_recent_files.removeAll(path);

	// Add to front
	_recent_files.prepend(path);

	// Keep only the last 10 files
	while (_recent_files.size() > 10)
	{
		_recent_files.removeLast();
	}

	save();
}

QString settings::last_directory() const
{
	return _last_directory;
}

void settings::set_last_directory(const QString& path)
{
	_last_directory = path;
	save();
}

void settings::load()
{
	const QSettings settings("Psychloor", "StylesManager");

	_recent_files = settings.value("recentFiles").toStringList();
	_last_directory = settings.value("lastDirectory").toString();
}

void settings::save() const
{
	QSettings settings("Psychloor", "StylesManager");

	settings.setValue("recentFiles", _recent_files);
	settings.setValue("lastDirectory", _last_directory);
}
