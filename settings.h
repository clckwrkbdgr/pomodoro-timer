#pragma once

class Settings {
public:
	enum { SECOND = 1000, MINUTE = 60 * SECOND };

	Settings();
	virtual ~Settings() {}

	void load();
	void save() const;

	bool getAutorestart() const;
	void setAutorestart(bool value);

	int getPomodoroLength() const;
	void setPomodoroLength(int value);
	int getBreakLength() const;
	void setBreakLength(int value);

	const QString & getStartCommand() const;
	void setStartCommand(const QString & value);
	const QString & getEndCommand() const;
	void setEndCommand(const QString & value);
private:
	bool autorestart;
	int pomodoroLength;
	int breakLength;
	QString startCommand;
	QString endCommand;
};

