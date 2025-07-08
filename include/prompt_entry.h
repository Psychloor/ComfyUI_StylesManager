//
// Created by blomq on 2025-07-07.
//

#ifndef PROMPT_ENTRY_H
#define PROMPT_ENTRY_H

#ifndef JSON_HAS_CPP_20
#define JSON_HAS_CPP_20
#endif

#include <third_party/json/json.h>

namespace ns
{
	struct prompt_entry
	{
		QString name;
		QString prompt;
		QString negative_prompt;

		bool operator==(const prompt_entry&) const = default;
		bool operator!=(const prompt_entry&) const = default;
	};

	template <typename TJson>
	void to_json(TJson& j, const prompt_entry& p)
	{
		j = TJson{
				{"name", p.name},
				{"prompt", p.prompt},
				{"negative_prompt", p.negative_prompt}
		};
	}

	template <typename TJson>
	void from_json(const TJson& j, prompt_entry& p)
	{
		j.at("name").get_to(p.name);
		j.at("prompt").get_to(p.prompt);
		j.at("negative_prompt").get_to(p.negative_prompt);
	}
}

// QString serialization (needs to be in nlohmann namespace)
NLOHMANN_JSON_NAMESPACE_BEGIN
template <>
struct adl_serializer<QString>
{
	template <typename TJson>
	static void to_json(TJson& j, const QString& q)
	{
		j = q.toUtf8().constData();
	}

	template <typename TJson>
	static void from_json(const TJson& j, QString& q)
	{
		q = QString::fromUtf8(j.template get<std::string>().c_str());
	}
};
NLOHMANN_JSON_NAMESPACE_END

#endif //PROMPT_ENTRY_H
