//
// Created by blomq on 2025-07-07.
//

#ifndef PROMPT_ENTRY_H
#define PROMPT_ENTRY_H

#ifndef JSON_HAS_CPP_20
#define JSON_HAS_CPP_20
#endif

#include <third_party/json/json.h>
#include <utility>

namespace ns
{
	struct prompt_entry
	{
		QString name;
		QString prompt;
		QString negative_prompt;

		prompt_entry() = default;

		prompt_entry(QString name, QString prompt, QString negative_prompt) : name(std::move(name)),
																			  prompt(std::move(prompt)),
																			  negative_prompt(
																				  std::move(negative_prompt))
		{
			if (this->prompt.isEmpty() || !this->prompt.contains("{prompt}"))
			{
				this->prompt = "{prompt}, " + prompt;
			}
		}

		bool operator==(const prompt_entry&) const = default;
		bool operator!=(const prompt_entry&) const = default;
	};

	template <typename TJson>
	void to_json(TJson& json, const prompt_entry& prompt)
	{
		json = TJson{
			{"name", prompt.name},
			{"prompt", prompt.prompt},
			{"negative_prompt", prompt.negative_prompt}
		};
	}

	template <typename TJson>
	void from_json(const TJson& json, prompt_entry& prompt)
	{
		json.at("name").get_to(prompt.name);
		json.at("prompt").get_to(prompt.prompt);
		json.at("negative_prompt").get_to(prompt.negative_prompt);
	}
}

// QString serialization (needs to be in nlohmann namespace)
NLOHMANN_JSON_NAMESPACE_BEGIN
	template <>
	struct adl_serializer<QString>
	{
		template <typename TJson>
		static void to_json(TJson& json, const QString& qstring)
		{
			json = qstring.toUtf8().constData();
		}

		template <typename TJson>
		static void from_json(const TJson& json, QString& qstring)
		{
			qstring = QString::fromUtf8(json.template get<std::string>().c_str());
		}
	};

NLOHMANN_JSON_NAMESPACE_END

#endif //PROMPT_ENTRY_H
