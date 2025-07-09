//
// Created by blomq on 2025-07-08.
//

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QStringList>

class settings {
public:
	static settings& instance();

	[[nodiscard]] QStringList recent_files() const;
	void add_recent_file(const QString& path);

	[[nodiscard]] QString last_directory() const;
	void set_last_directory(const QString& path);

private:
	settings() = default;

	void load();
	void save() const;

	QStringList _recent_files;
	QString _last_directory;
	bool _loaded = false;
};



#endif //SETTINGS_H
