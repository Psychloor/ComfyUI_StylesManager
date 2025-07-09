#ifndef PROMPT_NAME_VALIDATOR_H
#define PROMPT_NAME_VALIDATOR_H

#include <QValidator>

class prompt_name_validator final : public QValidator
{
	Q_OBJECT

public:
	explicit prompt_name_validator(QObject* parent = nullptr) : QValidator(parent)
	{
	}

	~prompt_name_validator() override = default;

	State validate(QString& input, int& pos) const override
	{
		Q_UNUSED(pos)

		if (input.isEmpty() || input.length() < 3)
		{
			return QValidator::Intermediate;
		}

		return QValidator::Acceptable;
	}

	void fixup(QString& input) const override
	{
	}
};

#endif //PROMPT_NAME_VALIDATOR_H
